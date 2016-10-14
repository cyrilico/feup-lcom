#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"

//Custom symbolic constants
#define GET_LSB (BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7))
#define GET_2ND_LSB (BIT(8) | BIT(9) | BIT(10) | BIT (11) | BIT(12) | BIT(13) | BIT(14) | BIT(15))

unsigned long interrupt_counter = 0;
int hookid = 15;

int timer_set_square(unsigned long timer, unsigned long freq) {

	//Read timer configuration
	unsigned char st;
	if(timer_get_conf(timer, &st)) //Value returned from timer_get_conf is not 0 so configuration was not retrieved successfully
		return -1;

	//Isolate 4 less significant bits so as to not change initial value interpretation and programming mode (as specified in the guide)
	st = st & (BIT(0) | BIT(1) | BIT(2) | BIT(3));

	unsigned long newfreq = TIMER_FREQ/freq;
	/*CHECK WITH PROFESSOR: "prettier" to define MSB and LSB macros; can we do it in i8254.h?*/
	unsigned long lsb_newfreq = newfreq & GET_LSB; //isolate LSB
	unsigned long msb_newfreq = newfreq & GET_2ND_LSB; //isolate MSB

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

int timer_subscribe_int(void ) {
	/*Bit specified in hookid to be returned by function if both calls are successful
	 (variable needed because hookid will be modified in case of successful call)*/
	int hookid_bit = BIT(hookid);
	switch(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hookid){
	case 0: //Success in setpolicy call
		if(!sys_irqenable(&hookid)) //Success in enable call
			return hookid_bit;
		else
			return -1;
	default: //Unsuccessful call, no need to do enable call
		return -1;
	}
}

int timer_unsubscribe_int() {
	switch(sys_irqdisable( &hookid){
	case 0: //Success in disable call
		if(!sys_irqrmpolicy(&hookid)) //Success in unsubscribe call
			return 0;
		else
			return -1;
	default: //Unsuccessful call, no need to do unsubscribe call
		return -1;
	}
}

void timer_int_handler() {
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
	printf("Programmed Mode: %x \n", (conf & (BIT(1) | BIT(2) | BIT(3)))>>1);
	printf("BCD: %x \n", conf & BIT(0));
	return 0;
}

int timer_test_square(unsigned long freq) {
	return timer_set_square(0,freq);
}

int timer_test_int(unsigned long time) {
	
	return 1;
}

int timer_test_config(unsigned long timer) {
	unsigned char st;
	if(!timer_get_conf(timer, &st)) //timer introduced is valid, so current configuration was retrieved and can be displayed
		timer_display_conf(st);
	else //timer introduced is not valid
		printf("Invalid timer! Value must be 0, 1 or 2\n");
	return 0;
}
