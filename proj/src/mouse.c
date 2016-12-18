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

/* --------------------- */

#define MOUSE_START_X 100
#define MOUSE_START_Y 100

Mouse* create_mouse(){
	Mouse* new_mouse = (Mouse*)(malloc(sizeof(Mouse)));
	new_mouse->left_button_state = RELEASED;
	new_mouse->crosshair = loadBitmap(fullPath("crosshair.bmp"),MOUSE_START_X,MOUSE_START_Y);
	new_mouse->byteID = 0;
	new_mouse->packet_byte = 0;
	new_mouse->packet_state = RECEIVING;

	mouse_write_byte(ENABLE_MOUSE_DATA_REPORTING);

	return new_mouse;
}

Mouse* create_game_mouse() {
	Mouse* new_mouse = (Mouse*)(malloc(sizeof(Mouse)));
	new_mouse->left_button_state = RELEASED;
	new_mouse->byteID = 0;
	new_mouse->packet_byte = 0;
	new_mouse->packet_state = RECEIVING;

	mouse_write_byte(ENABLE_MOUSE_DATA_REPORTING);

	return new_mouse;
}

void draw_mouse(Mouse* mouse, char* buffer){
	drawBitmap(mouse->crosshair, buffer, ALIGN_LEFT);
}

void read_packet_byte(Mouse* mouse){
	sys_inb(OUT_BUF, &(mouse->packet_byte));
	if(mouse->byteID == 0){
		if(mouse->packet_byte & BIT(3)) //Synchronize. If <> 0, it's a valid first byte (or at least we hope so)
			mouse->packet[mouse->byteID++] = (unsigned char)(mouse->packet_byte);
		else //Off-sync, so ignore byte and wait for a synced packet
			return;
	}
	else{
		mouse->packet[mouse->byteID++] = (unsigned char)(mouse->packet_byte);
		if(mouse->byteID == 3){
			//mouse_print_packet(mouse->packet);
			mouse->byteID = 0;
			update_mouse(mouse);
			mouse->packet_state = RECEIVED;
		}
	}
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

	//Limit mouse to screen
	if(mouse->crosshair->x < 0)
		mouse->crosshair->x = 0;
	if(mouse->crosshair->y < 0)
		mouse->crosshair->y = 0;

	if(mouse->crosshair->x > vg_get_h_res() - mouse->crosshair->bitmapInfoHeader.width)
		mouse->crosshair->x = vg_get_h_res() - mouse->crosshair->bitmapInfoHeader.width;
	if(mouse->crosshair->y > vg_get_v_res() - mouse->crosshair->bitmapInfoHeader.height)
		mouse->crosshair->y =  vg_get_v_res() - mouse->crosshair->bitmapInfoHeader.height;
}

int full_packet_received(Mouse* mouse){
	if(mouse->packet_state == RECEIVED)
		return 1;
	else
		return 0;
}

void reset_packet_state(Mouse* mouse){
	mouse->packet_state = RECEIVING;
}

void delete_mouse(Mouse* mouse){
	deleteBitmap(mouse->crosshair);
	mouse_write_byte(DISABLE_MOUSE_DATA_REPORTING);
	free(mouse);
}

void delete_game_mouse(Mouse* mouse) {
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
