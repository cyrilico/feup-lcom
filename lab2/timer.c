#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"

unsigned long interrupt_counter = 0;
int hookid = 15;

int timer_set_square(unsigned long timer, unsigned long freq) {

	//Read timer configuration
	//Value returned from timer_get_conf is not 0 so configuration was not retrieved successfully OR given freq is not valid
	unsigned char st;
	if(timer_get_conf(timer, &st) != OK || freq == 0)
		return -1;

	//Isolate 4 less significant bits so as to not change initial value interpretation and programming mode (as specified in the guide)
	st = st & (BIT(0) | BIT(1) | BIT(2) | BIT(3));

	unsigned long newfreq = TIMER_FREQ/freq;
	unsigned long lsb_newfreq = newfreq & GET_LSB; //isolate LSB
	unsigned long msb_newfreq = (newfreq & GET_2ND_LSB)>>8; //isolate MSB

	switch(timer){ //If function reaches this stage given timer should be valid so no need for default case
	case 0:
		sys_outb(TIMER_CTRL, TIMER_SEL0 | TIMER_LSB_MSB | st);
		sys_outb(TIMER_0, lsb_newfreq);
		sys_outb(TIMER_0, msb_newfreq);
		return 0;
	case 1:
		sys_outb(TIMER_CTRL, TIMER_SEL1 | TIMER_LSB_MSB | st);
		sys_outb(TIMER_1, lsb_newfreq);
		sys_outb(TIMER_1, msb_newfreq);
		return 0;
	case 2:
		sys_outb(TIMER_CTRL, TIMER_SEL2 | TIMER_LSB_MSB | st);
		sys_outb(TIMER_2, lsb_newfreq);
		sys_outb(TIMER_2, msb_newfreq);
		return 0;
	}

}

int timer_subscribe_int(void) {
	/*Bit specified in hookid to be returned by function if both calls are successful
	 (variable needed because hookid will be modified in case of successful call)*/
	int hookid_bit = BIT(hookid);

	switch(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hookid)) {
		case 0: //Success in setpolicy call
			if(sys_irqenable(&hookid) == OK) //Success in enable call
				return hookid_bit;
		default: //Unsuccessful call, no need to do enable call
			return -1;
	}
}

int timer_unsubscribe_int() {
	switch(sys_irqdisable( &hookid)) {
		case 0: //Success in disable call
			if(sys_irqrmpolicy(&hookid) == OK) //Success in unsubscribe call
				return 0;
		default: //Unsuccessful call, no need to do unsubscribe call
			return -1;
	}
}

void timer_int_handler() {
	//Increment global counter variable
	interrupt_counter++;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	
	unsigned long st_temp; /* because argument in sys_inb is of type unsigned long* and st is not of that type*/

	switch(timer) {
		case 0:
			sys_outb(TIMER_CTRL, TIMER_RB_CMD | BIT(5) | BIT(1)); /* Read-back, STATUS active, Timer0 */
			sys_inb(TIMER_0, &st_temp);
			*st = (unsigned char)st_temp;
			return 0;
		case 1:
			sys_outb(TIMER_CTRL, TIMER_RB_CMD | BIT(5) | BIT(2)); /* Read-back, STATUS active, Timer1 */
			sys_inb(TIMER_1, &st_temp);
			*st = (unsigned char)st_temp;
			return 0;
		case 2:
			sys_outb(TIMER_CTRL, TIMER_RB_CMD | BIT(5) | BIT(3)); /* Read-back, STATUS active, Timer2 */
			sys_inb(TIMER_2, &st_temp);
			*st = (unsigned char)st_temp;
			return 0;
		default:
			return -1;

	}

}

int timer_display_conf(unsigned char conf) {
	
	printf("Timer status byte: 0x%x \n", conf);
	printf("Output value: %x \n", conf & BIT(7));
	printf("Null count: %x \n", conf & BIT(6));
	printf("Access type: ");
	switch(conf & TIMER_LSB_MSB) {
		case TIMER_LSB:
			printf("LSB \n");
			break;
		case TIMER_MSB:
			printf("MSB \n");
			break;
		case TIMER_LSB_MSB:
			printf("LSB followed by MSB \n");
			break;
		default:
			printf("Not found \n"); //When config is from timer 1 or 2 these bits come as 00, so just print out a "random" message (told by professor)
			break;
	}
	/*Bitwise ORs followed by AND to obtain programmed mode bits. Next, a shift right of 1 bit so the value is not misinterpreted with an extra bit */
	printf("Programmed Mode: ");
	 switch((conf & (BIT(1) | BIT(2) | BIT(3)))>>1) {
		 case 0:
			 printf("0 (interrupt on terminal count)\n");
			 break;
		 case 1:
			 printf("1 (hardware retriggerable one-shot)\n");
			 break;
		 case 2:
			 printf("2 (rate generator)\n");
			 break;
		 case 3:
			 printf("3 (square wave generator)\n");
			 break;
	 }

	printf("Counting mode: ");
	 switch(conf & BIT(0)) {
		 case 0:
			 printf("0 (binary)\n");
			 break;
		 case 1:
			 printf("1 (BCD)\n");
			 break;
	 }
	return 0;
}

int timer_test_square(unsigned long freq) {
	return timer_set_square(0,freq);
}

int timer_test_int(unsigned long time) {
	
	int r;
	int irq_set = timer_subscribe_int();

	if(irq_set == -1) //Failed subscription
		return -1;

	int ipc_status;
	message msg;

	while(interrupt_counter/60 < time) { /* You may want to use a different condition */
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					timer_int_handler();
					//If counter mod 60 == 0, timer0 has made another 60 interruptions so another second has passed
					if(interrupt_counter % 60 == 0)
						printf("Hey! Another second has passed. %d seconds gone so far\n", interrupt_counter/60);

				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	return timer_unsubscribe_int();
}

int timer_test_config(unsigned long timer) {
	unsigned char st;
	if(!timer_get_conf(timer, &st)) //timer introduced is valid, so current configuration was retrieved and can be displayed
		timer_display_conf(st);
	else //timer introduced is not valid
		printf("Invalid timer! Value must be 0, 1 or 2\n");
	return 0;
}
