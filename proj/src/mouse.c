#include "mouse.h"

#define MOUSE_WIDTH 10
#define MOUSE_HEIGHT 10

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
			*st = DRAW;
		}
		break;
	case DRAW:
		if( evt == MOVE ) {
			if(desired_length < 0) {
				if (*y_variation <= desired_length){ //Make length a global variable so it can be accessed here?
					*st = COMP;
				}
			}
			else{
				if (*y_variation >= desired_length){ //Make length a global variable so it can be accessed here?
					*st = COMP;
				}
			}
		} else if( evt == RUP ) {
			*st = INIT;
			*y_variation = 0;
		}
		break;
	default:
		break;
	}
}

/* --------------------- */

Mouse* create_mouse(){
	Mouse* new_mouse = (Mouse*)(malloc(sizeof(Mouse)));
	new_mouse->left_button_state = RELEASED;
	new_mouse->crosshair = loadBitmap(fullPath("crosshair.bmp"),100,100); //Mouse starts at (x,y)=(100,100)
	new_mouse->byteID = 0;
	new_mouse->packet_byte = 0;
	new_mouse->number_of_packets = 0;
	mouse_write_byte(ENABLE_MOUSE_DATA_REPORTING);
	return new_mouse;
}

void draw_mouse(Mouse* mouse, char* buffer){
	if(mouse->crosshair->x < 0)
		mouse->crosshair->x = 0;
	if(mouse->crosshair->y < 0)
		mouse->crosshair->y = 0;

	if(mouse->crosshair->x > vg_get_h_res() - MOUSE_WIDTH)
		mouse->crosshair->x = vg_get_h_res() - MOUSE_WIDTH;
	if(mouse->crosshair->y > vg_get_v_res() - MOUSE_HEIGHT)
		mouse->crosshair->y =  vg_get_v_res() - MOUSE_HEIGHT;
	drawBitmap(mouse->crosshair, buffer, ALIGN_LEFT);
}

void update_mouse(Mouse* mouse){
	if(mouse->packet[0] & BIT(4)) //Negative x delta
		mouse->crosshair->x -= ABS_VALUE(TWOSCOMPLEMENT(mouse->packet[1]));
	else //Positive x delta
		mouse->crosshair->x += mouse->packet[1];
	if(mouse->packet[0] & BIT(5)) //Negative y delta
		mouse->crosshair->y += ABS_VALUE(TWOSCOMPLEMENT(mouse->packet[2]));
	else //Positive y delta
		mouse->crosshair->y -= mouse->packet[2];
	if(mouse->packet[0] & BIT(0)){ //Left button pressed
		if(mouse->left_button_state == RELEASED)
			mouse->left_button_state = PRESSED;
	}
	else{
		if(mouse->left_button_state == PRESSED){
			mouse->left_button_state = RELEASED;
			mouse->left_button_was_released = 1;
		}
		else
			mouse->left_button_was_released = 0;
	}
}

void delete_mouse(Mouse* mouse){
	deleteBitmap(mouse->crosshair);
	mouse_write_byte(DISABLE_MOUSE_DATA_REPORTING);
	free(mouse);
}

void mouse_print_packet(unsigned char packet[]){
	printf("B1=0x%x ", packet[0]);
	printf("B2=0x%x ", packet[1]);
	printf("B3=0x%x ", packet[2]);
	printf("LB=%u ", packet[0] & BIT(0));
	printf("MB=%u ", (packet[0] & BIT(2))>>2);
	printf("RB=%u ", (packet[0] & BIT(1))>>1);
	printf("XOV=%u ", (packet[0] & BIT(6))>>6);
	printf("YOV=%u ", (packet[0] & BIT(7))>>7);
	printf("X=%d ", ((packet[0] & BIT(4)) ? TWOSCOMPLEMENT(packet[1]) : packet[1]));
	printf("Y=%d\n\n", ((packet[0] & BIT(5)) ? TWOSCOMPLEMENT(packet[2]) : packet[2]));

}
