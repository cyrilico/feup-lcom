#include "mouse.h"

int hookid_mouse = 23;

int mouse_subscribe_int() {
	/*Variable that will hold return value in case of successful call, since sys_irq calls will modify hookid value*/
	int hookid_mouse_bit = BIT(hookid_mouse);

	if(sys_irqsetpolicy(IRQ_MOUSE, IRQ_EXCLUSIVE | IRQ_REENABLE, &hookid_mouse) != OK)
		return -1;

	if(sys_irqenable(&hookid_mouse) != OK)
		return -1;

	return hookid_mouse_bit;
}

int mouse_unsubscribe_int() {
	if(sys_irqdisable(&hookid_mouse) != OK)
		return -1;

	if(sys_irqrmpolicy(&hookid_mouse) != OK)
		return -1;

	return 0;
}

unsigned long mouse_read_code(){
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

unsigned long mouse_write_code(long destination, unsigned char cmd){
	unsigned long st;
	unsigned int counter = 0;
	while( counter <= NTRIES ) {
		sys_inb(STAT_REG, &st); /* assuming it returns OK */
		/* loop while 8042 input buffer is not empty */
		if( (st & IBF) == 0 ) {
			sys_outb(destination, cmd); /* no args command */
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
	return -1;
}

int mouse_write_byte(unsigned char byte){
	unsigned long trash, counter = 0;

	do{
		if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
			return -1;

		if(mouse_write_code(IN_BUF, byte) == -1)

			return -1;
		sys_inb(OUT_BUF, &trash);
		if(trash != ACK)
			printf("Error sending 0x%x, trying again\n",byte);
		counter++;
	}while(trash != ACK && counter < NTRIES);
	return 0;
}

void mouse_event_handler(state *st, event evt, short *y_variation, short desired_length) {
	switch (*st) {
	case INIT:
		if( evt == RDOWN ){
			printf("State change: is now DRAW\n");
			*st = DRAW;
		}
		break;
	case DRAW:
		if( evt == MOVE ) {
			printf("Testing movement\n");
			printf("Current total movement: %d\n Total expected movement: %d\n", *y_variation, desired_length);

			if(desired_length < 0) {
				if (*y_variation <= desired_length){ //Make length a global variable so it can be accessed here?
					printf("Completed, exiting\n");
					*st = COMP;
				}
			}
			else{
				if (*y_variation >= desired_length){ //Make length a global variable so it can be accessed here?
					printf("Completed, exiting\n");
					*st = COMP;
				}
			}
		} else if( evt == RUP ) {
			printf("State change: is now INIT\n");
			*st = INIT;
			*y_variation = 0;
		}
		break;
	default:
		break;
	}
}
