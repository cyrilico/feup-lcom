#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "keyboard.h"
#include "timer.h"
#include "i8254.h"
#include "i8042.h"

int hookid_kbd = 10;
int SCROLLLOCK_ON = 0;
int NUMLOCK_ON = 0;
int CAPSLOCK_ON = 0;

int kbd_subscribe_int() {
	/*Variable that will hold return value in case of successful call, since sys_irq calls will modify hookid value*/
	int hookid_kbd_bit = BIT(hookid_kbd);

	if(sys_irqsetpolicy(IRQ_KBD, IRQ_EXCLUSIVE | IRQ_REENABLE, &hookid_kbd) != OK)
		return -1;

	if(sys_irqenable(&hookid_kbd) != OK)
		return -1;

	return hookid_kbd_bit;
}

int kbd_unsubscribe_int() {

	if(sys_irqdisable(&hookid_kbd) != OK)
		return -1;

	if(sys_irqrmpolicy(&hookid_kbd) != OK)
		return -1;

	return 0;
}

unsigned long kbd_read_code() {

	unsigned long st;
	unsigned long data;
	unsigned int counter = 0;

	while(counter <= NTRIES) {
		sys_inb(STAT_REG, &st);
		/* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if(st & OBF) {
			sys_inb(OUT_BUF, &data); /* assuming it returns OK */
			if ( (st &(PARITY | TIMEOUT)) == 0 )
				return data;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		counter++;
	}
	return -1;
}

unsigned long kbd_write_code(unsigned char cmd) {
	unsigned long st;
	unsigned int counter = 0;
	while( counter <= NTRIES ) {
		sys_inb(STAT_REG, &st); /* assuming it returns OK */
		/* loop while 8042 input buffer is not empty */
		if( (st & IBF) == 0 ) {
			sys_outb(IN_BUF, cmd); /* no args command */
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
	return -1;
}

void kbd_print_code(unsigned long code) {
	if(code & GET_MSB){ //Scancode has two bytes
		if(code & BIT(15)) //Breakcode
			printf("Breakcode: 0x%04x\n", code);
		else
			printf("Makecode: 0x%04x\n", code);
	}
	else{ //Scancode has one byte
		if(code & BIT(7)) //Breakcode
			printf("Breakcode: 0x%02x\n", code);
		else
			printf("Makecode: 0x%02x\n", code);
	}
}

int kbd_scan_loop(unsigned short c_or_asm) {

	unsigned long lixo;
	int r;
	int irq_set = kbd_subscribe_int();

	if(irq_set == -1) //Failed subscription
		return -1;

	int ipc_status;
	message msg;

	unsigned long code = 0;
	unsigned long code_aux = 0; //Auxiliar variable in case of two byte scancode
	int read_again = 0;

	while(code != ESC_BREAK) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					if(c_or_asm == 0)
						code = kbd_read_code();
					else
						code = kbd_read_code_ASM();
					if(code == -1)
						return -1;
					else if(read_again == 1){
						code = code << 8;
						code |= code_aux;
						read_again = 0;
						kbd_print_code(code);
					}
					else if(code == TWO_BYTE_SCANCODE){
						code_aux = code;
						read_again = 1;
					}
					else
						kbd_print_code(code);
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	return kbd_unsubscribe_int();
}

int kbd_timed_scan_loop(unsigned short n){
	int r;
	int kbd_irq_set = kbd_subscribe_int();
	if(kbd_irq_set == -1) //Failed keyboard subscription
		return -1;
	int timer_irq_set = timer_subscribe_int();
	if(timer_irq_set == -1) //Failed timer subscription
		return -1;

	int ipc_status;
	message msg;

	unsigned long code = 0;
	unsigned long code_aux = 0; //Auxiliar variable in case of two byte scancode
	int read_again = 0;
	unsigned int counter = 0;

	while(code != ESC_BREAK && counter/60 < n) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & kbd_irq_set) {
					code = kbd_read_code();
					if(code == -1)
						return -1;
					else if(read_again == 1){
						code = code << 8;
						code |= code_aux;
						read_again = 0;
						kbd_print_code(code);
					}
					else if(code == TWO_BYTE_SCANCODE){
						code_aux = code;
						read_again = 1;
					}
					else
						kbd_print_code(code);

					counter = 0; //Keyboard has caused an interrupt so idle time is reset
				}
				else if(msg.NOTIFY_ARG & timer_irq_set) {
					counter++;
					if(counter % 60 == 0)
						printf("No keyboard action detected in the last second. %ds until program exits.\n", n-counter/60);
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	if(kbd_unsubscribe_int() == -1 || timer_unsubscribe_int() == -1)
		return -1;
	else
		return 0;
}

int kbd_send_led_cmd(){
	unsigned long response;
	do{
		if(kbd_write_code(PROCESS_LEDS) == -1)
			return -1;
		response = kbd_read_code();
		if(response == -1)
			return -1;
	}while(response == ERROR || response == RESEND);
	return 0;
}

int kbd_process_leds(unsigned long led_sequence){
	int ready_to_process;
	int response;
	do{
		ready_to_process = kbd_send_led_cmd();
		if(ready_to_process == -1)
			return -1;
		do{
			if(kbd_write_code(led_sequence) == -1)
				return -1;
			if( (response = kbd_read_code()) == -1)
				return -1;
			if (response == ERROR || response == ACK)
				break;
		}while(response != ERROR && response != ACK);
		/*if(response == ACK) //Redundant since it will test if it's ERROR in line 245 (if it's not ERROR it's ACK and it will exit
			break;
		*/
	}while(response == ERROR);
	return 0;
}

unsigned long kbd_change_led_sequence(unsigned long led){
	switch(led){
	case 0:
		SCROLLLOCK_ON = (SCROLLLOCK_ON+1)%2; //Guarantees that if it was 1 it goes to 0 and vice-versa
		break;
	case 1:
		NUMLOCK_ON = (NUMLOCK_ON+1)%2;
		break;
	case 2:
		CAPSLOCK_ON = (CAPSLOCK_ON+1)%2;
		break;
	}
	return ((CAPSLOCK_ON << 2) | (NUMLOCK_ON << 1) | SCROLLLOCK_ON);
}


/*Assumes all leds are turned off in the beginning (should it not?)*/
int kbd_leds_loop(unsigned short n, unsigned short *leds){
		int ipc_status;
	int r;
	message msg;
	unsigned int counter = 0;
	int index = 0;
	int led_sequence = 0;
	int irq_set = timer_subscribe_int();
	if(irq_set == -1) //Failed subscription
		return -1;
	int kbd_subscribe = kbd_subscribe_int();
	if(kbd_subscribe == -1)
		return -1;
	unsigned long trash = kbd_read_code();
	while(counter/60 < n) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					counter++;
					if(counter % 60 == 0){ //Another second has gone by
						led_sequence = kbd_change_led_sequence(leds[index]);
						printf("Current led sequence: 0x%x\n", led_sequence);
						kbd_process_leds(led_sequence);
						index++;
						printf("Another second has passed, current led: %d\n", leds[index-1]);
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if(kbd_unsubscribe_int() == -1 || timer_unsubscribe_int() == -1)
		return -1;
	else{
		printf("Exiting\n");
		return 0;
	}
}
