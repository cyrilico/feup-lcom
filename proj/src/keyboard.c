#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "keyboard.h"
#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include "utils.h"

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

/*------------------------------------------------*/

Keyboard* create_keyboard(){
	Keyboard* keyboard = (Keyboard*)(malloc(sizeof(Keyboard)));
	keyboard->scancode = 0;
	keyboard->scancode_aux = 0;
	keyboard->read_again = 0;

	return keyboard;
}

void read_scancode(Keyboard* keyboard){
	unsigned long code = read_scancode_asm();
	if(code != -1){
		if(keyboard->read_again == 1){
			code = code << 8;
			code |= keyboard->scancode_aux;
			keyboard->scancode = code;
			keyboard->read_again = 0;
		}
		else if(code == TWO_BYTE_SCANCODE){
			keyboard->scancode_aux = code;
			keyboard->read_again = 1;
		}
		else
			keyboard->scancode = code;
	}
}

int full_scancode_received(Keyboard* keyboard){
	if(keyboard->read_again == 0)
		return 1;
	else
		return 0;
}

int key_detected(Keyboard* keyboard, unsigned long key){
	if(full_scancode_received(keyboard) && keyboard->scancode == key)
		return 1;
	else
		return 0;
}

void delete_keyboard(Keyboard* keyboard){
	free(keyboard);
}
