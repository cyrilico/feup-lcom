#include "mouse.h"
#include "timer.h"

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

int test_packet(unsigned short cnt){
	int r;
	int irq_set = mouse_subscribe_int();

	if(irq_set == -1) //Failed subscription
		return -1;

	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	unsigned char packet[3];
	long byte; //Auxiliar variable that will store each byte read

	do{
		if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
			return -1;
		if(mouse_write_code(IN_BUF, ENABLE_MOUSE_DATA_REPORTING) == -1)
			return -1;
		sys_inb(OUT_BUF, &byte);
		if(byte != ACK)
			printf("Erro a mandar F4\n");
	}while(byte != ACK);

	printf("Value returned after data reporting enabled: 0x%x\n\n", byte);
	/*Testing if TWOSCOMPLEMENT works*/
	printf("1 - %ld\n2 - %ld\n3 - %ld\n", TWOSCOMPLEMENT(1), TWOSCOMPLEMENT(2), TWOSCOMPLEMENT(3));

	while(number_of_packets < cnt) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					sys_inb(OUT_BUF, &byte);
					if(counter == 0){
						if(byte & BIT(3)) //Valid first byte (or at least we hope so)
							packet[counter++] = (unsigned char)byte;
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						packet[counter++] = (unsigned char)byte;
						if(counter > 2){
							mouse_print_packet(packet);
							counter = 0;
							number_of_packets++;
						}
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

	do{
		if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
			return -1;
		if(mouse_write_code(IN_BUF, DISABLE_MOUSE_DATA_REPORTING) == -1)
			return -1;
		sys_inb(OUT_BUF, &byte);
		if(byte != ACK)
			printf("Erro a mandar F%\n");
	}while(byte != ACK);

	return mouse_unsubscribe_int();
}

int test_async(unsigned short idle_time) {
	int r;
	int irq_set = mouse_subscribe_int();

	if(irq_set == -1) //Failed subscription
		return -1;

    int timer_irq_set = timer_subscribe_int();
    if(timer_irq_set == -1) //Failed timer subscription
    	 return -1;

	int ipc_status;
	message msg;

	unsigned short counter = 0;
	unsigned char packet[3];
	long byte; //Auxiliar variable that will store each byte read

	unsigned int idle_counter = 0;

	do{
		if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
			return -1;
		if(mouse_write_code(IN_BUF, ENABLE_MOUSE_DATA_REPORTING) == -1)
			return -1;
		sys_inb(OUT_BUF, &byte);
		if(byte != ACK)
			printf("Erro a mandar F4\n");
	}while(byte != ACK);

	printf("Value returned after data reporting enabled: 0x%x\n\n", byte);
	if(byte != ACK)
		return -1;
	/*Testing if TWOSCOMPLEMENT works*/
	printf("1 - %ld\n2 - %ld\n3 - %ld\n", TWOSCOMPLEMENT(1), TWOSCOMPLEMENT(2), TWOSCOMPLEMENT(3));

	while(idle_counter/60 < idle_time) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					sys_inb(OUT_BUF, &byte);
					idle_counter = 0; //Mouse has caused an interrupted so idle time is reset
					if(counter == 0){
						if(byte & BIT(3)) { //Valid first byte (or at least we hope so)
							packet[counter++] = (unsigned char)byte;
						}
						else{
							printf("Invalid first byte so invalid packet, trying again\n\n");
							continue;
						}
					}
					else{
						packet[counter++] = (unsigned char)byte;
						if(counter > 2) {
							mouse_print_packet(packet);
							counter = 0;
						}
					}
				}
				else if(msg.NOTIFY_ARG & timer_irq_set) {
					idle_counter++;
					if(idle_counter % 60 == 0)
						printf("No mouse action detected in the last second. %ds until program exits.\n", idle_time-idle_counter/60);
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	do{
		if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
			return -1;
		if(mouse_write_code(IN_BUF, DISABLE_MOUSE_DATA_REPORTING) == -1)
			return -1;
		sys_inb(OUT_BUF, &byte);
		if(byte != ACK)
			printf("Erro a mandar F5\n");
	}while(byte != ACK);

	if(mouse_unsubscribe_int() == -1 || timer_unsubscribe_int() == -1)
		return -1;
	else
		return 0;
}

void mouse_print_config(unsigned char packet[]){
	printf("Current mouse configuration:\n");
	printf("Mode: %s", (packet[0] & BIT(6)) ? "Remote (polled)\n" : "Stream\n");
	printf("Data reporting is %s", (packet[0] & BIT(5)) ? "enabled\n" : "disabled\n");
	printf("Scaling is %s", (packet[0] & BIT(4)) ? "2:1\n" : "1:1\n");
	printf("Middle button is %s", (packet[0] & BIT(2)) ? "pressed\n" : "released\n");
	printf("Right button is %s", (packet[0] & BIT(1)) ? "pressed\n" : "released\n");
	printf("Left button is %s", (packet[0] & BIT(0)) ? "pressed\n" : "released\n");
	printf("Resolution: %d counts per mm\n", BIT(packet[1])); //packet[1] returns 0,1,2 or 3. The corresponding value is 2^value. BIT(n) does that
	printf("Sample rate: %d Hz\n\n", packet[2]);
}

int test_config(void) {
	if(mouse_subscribe_int() == -1) //Failed subscription
		return -1;

	unsigned short counter = 0;
	unsigned char packet[3];
	long byte; //Auxiliar variable that will store each byte read
	do{
			if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
				return -1;
			if(mouse_write_code(IN_BUF, ENABLE_MOUSE_DATA_REPORTING) == -1)
				return -1;
			sys_inb(OUT_BUF, &byte);
			if(byte != ACK)
				printf("Erro a mandar F4\n");
		}while(byte != ACK);
	do{
		if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
			return -1;
		if(mouse_write_code(IN_BUF, GET_MOUSE_CONFIG) == -1)
			return -1;
		sys_inb(OUT_BUF, &byte);
		if(byte != ACK)
			printf("Erro a mandar E9\n");
	}while(byte != ACK);

	while(counter < 3){
		sys_inb(OUT_BUF, &byte);
		packet[counter++] = (unsigned char)byte;
		tickdelay(micros_to_ticks(DELAY_US));
	}

	mouse_print_config(packet);
	return mouse_unsubscribe_int();
}


int test_gesture(short length) {
	int r;
	int irq_set = mouse_subscribe_int();

	if(irq_set == -1) //Failed subscription
		return -1;

	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	unsigned char packet[3];
	long byte; //Auxiliar variable that will store each byte read

	int mouse_event = 0;
	rbstate previousrb = ISDOWN;

	short desired_length = length;
	short y_variation = 256;
	state st = INIT; //Variable that will hold machine state
	int sign_change = 0;

	do{
		if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
			return -1;
		if(mouse_write_code(IN_BUF, ENABLE_MOUSE_DATA_REPORTING) == -1)
			return -1;
		sys_inb(OUT_BUF, &byte);
		if(byte != ACK)
			printf("Erro a mandar F4\n");
	}while(byte != ACK);

	printf("Value returned after data reporting enabled: 0x%x\n\n", byte);

	while(st != COMP) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					sys_inb(OUT_BUF, &byte);
					if(counter == 0){
						if(byte & BIT(3)) //Valid first byte (or at least we hope so)
							packet[counter++] = (unsigned char)byte;
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						packet[counter++] = (unsigned char)byte;
						if(counter > 2){
							mouse_print_packet(packet);
							counter = 0;
							mouse_event = 1;
						}
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
		if(mouse_event){
			mouse_event = 0;
			if(!(packet[0] & BIT(1)) && previousrb == ISDOWN){//Right button was pressed but got released
				previousrb = ISUP;
				mouse_event_handler(&st, RUP, &y_variation, desired_length, &sign_change);
			}
			else if((packet[0] & BIT(1)) && previousrb == ISDOWN){ //Right button was and is pressed
				sign_change = ((packet[0] & BIT(4))>>4) ^ ((packet[0] & BIT(5))>>5); //If the x and y variation have different signs, reject "drawing"
				if(packet[0] & BIT(5))
					y_variation += TWOSCOMPLEMENT(packet[2]);
				else
					y_variation += packet[2];
			mouse_event_handler(&st, MOVE, &y_variation, desired_length, &sign_change);
			}
			else if((packet[0] & BIT(1)) && previousrb == ISUP){ //Right button was released but got pressed
				previousrb = ISDOWN;
				mouse_event_handler(&st, RDOWN, &y_variation, desired_length, &sign_change);
			}
		}
	}

	do{
		if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
			return -1;
		if(mouse_write_code(IN_BUF, DISABLE_MOUSE_DATA_REPORTING) == -1)
			return -1;
		sys_inb(OUT_BUF, &byte);
		if(byte != ACK)
			printf("Erro a mandar F5\n");
	}while(byte != ACK);

	return mouse_unsubscribe_int();
}
