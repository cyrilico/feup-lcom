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

void mouse_event_handler(state *st, event evt, short *y_variation, short desired_length, int *sign_change) {
	switch (*st) {
	case INIT:
		if( evt == RDOWN ){
			printf("Mudanca de estado para DRAW\n");
			*st = DRAW;
		}
		break;
	case DRAW:
		if( evt == MOVE ) {
			printf("Testando movimento\n");
			printf("Movimento total atual: %d\n Movimento total esperado: %d\n", *y_variation, desired_length);
			//Test if movement is enough to complete desired length. If so, st = COMP
			if(*sign_change == 1) { //Reject movement if x and y variations have different signs
				st = INIT;
				*y_variation = 0;
				*sign_change = 0;
			}
			if(desired_length < 0){
				if (*y_variation <= desired_length){ //Make length a global variable so it can be accessed here?
					printf("Completou\n");
					*st = COMP;
				}
			}
			else{
				if (*y_variation >= desired_length){ //Make length a global variable so it can be accessed here?
					printf("Completou\n");
					*st = COMP;
				}
			}
		} else if( evt == RUP ) {
			printf("Mudança de estado para INIT\n");
			*st = INIT;
			*y_variation = 0;
			//sign_change = 0;
		}
		break;
	default:
		break;
	}
}
