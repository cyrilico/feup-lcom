#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"

int timer_set_square(unsigned long timer, unsigned long freq) {

	//Read timer configuration
	unsigned char st;
	if(timer_get_conf(timer, &st)) //Value returned from timer_get_conf is not 0 so configuration was not retrieved successfully
		return -1;

	//Logical shifts to isolate 4 least significant bits of read configuration
	st<<4;
	st>>4;

	unsigned lsb_freq = freq; // 8 LSB from the new freq
	freq>>8; //8 bit right shift to get 8 MSB on the right side
	unsigned msb_freq = freq; // 8 MSB from the new freq

	switch(timer){ //If function reaches this stage given timer should be valid so no need for default case
	case 0:
		sys_outb(TIMER_CTRL, TIMER_SEL0 | TIMER_LSB_MSB | st);
		sys_outb(TIMER_0, lsb_freq);
		sys_outb(TIMER_0, msb_freq);
		break;
	case 1:
		sys_outb(TIMER_CTRL, TIMER_SEL1 | TIMER_LSB_MSB | st);
		sys_outb(TIMER_1, lsb_freq);
		sys_outb(TIMER_1, msb_freq);
		break;
	case 2:
		sys_outb(TIMER_CTRL, TIMER_SEL2 | TIMER_LSB_MSB | st);
		sys_outb(TIMER_2, lsb_freq);
		sys_outb(TIMER_2, msb_freq);
		break;
	}

}

int timer_subscribe_int(void ) {

	return 1;
}

int timer_unsubscribe_int() {

	return 1;
}

void timer_int_handler() {

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
	
	return 1;
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
