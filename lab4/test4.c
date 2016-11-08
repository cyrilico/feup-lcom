#include "mouse.h"
#include "timer.h"

void mouse_print_packet(long packet[]){
	printf("B1=0x%x ", packet[0]);
	printf("B2=0x%x ", packet[1]);
	printf("B3=0x%x ", packet[2]);
	printf("LB=%u ", packet[0] & BIT(0));
	printf("MB=%u ", packet[0] & BIT(2));
	printf("RB=%u ", packet[0] & BIT(1));
	printf("XOV=%u ", packet[0] & BIT(6));
	printf("YOV=%u ", packet[0] & BIT(7));
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
	int counter = 0;
	long packet[3];
	long byte; //Auxiliar variable that will store each byte read

	if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
		return -1;
	if(mouse_write_code(IN_BUF, ENABLE_MOUSE_DATA_REPORTING) == -1)
		return -1;

	sys_inb(OUT_BUF, &byte);
	if("Value returned after data reporting enabled: 0x%x\n\n", byte);
	if(byte != ACK)
		return -1;
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
							packet[counter++] = byte;
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						packet[counter++] = byte;
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

	if(mouse_write_code(STAT_REG, WRITE_BYTE_MOUSE) == -1)
		return -1;
	if(mouse_write_code(IN_BUF, DISABLE_MOUSE_DATA_REPORTING) == -1)
		return -1;
	sys_inb(OUT_BUF, &byte); //Make sure nothing stays in OUT_BUF
	return mouse_unsubscribe_int();
}

int test_async(unsigned short idle_time) {
    /* To be completed ... */

	int r;
	int irq_set = mouse_subscribe_int();

	if(irq_set == -1) //Failed subscription
		return -1;

    int timer_irq_set = timer_subscribe_int();
    if(timer_irq_set == -1) //Failed timer subscription
    	 return -1;

	int ipc_status;
	message msg;

	int counter = 0;
	long packet[3];
	long byte; //Auxiliar variable that will store each byte read

	unsigned int idle_counter = 0;

	if(mouse_write_code(STAT_REG,WRITE_BYTE_MOUSE) == -1)
		return -1;
	if(mouse_write_code(IN_BUF, ENABLE_MOUSE_DATA_REPORTING) == -1)
		return -1;

	sys_inb(OUT_BUF, &byte);
	if("Value returned after data reporting enabled: 0x%x\n\n", byte);
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
							packet[counter++] = byte;
						}
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						packet[counter++] = byte;
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

	if(mouse_write_code(STAT_REG, WRITE_BYTE_MOUSE) == -1)
		return -1;

	if(mouse_write_code(IN_BUF, DISABLE_MOUSE_DATA_REPORTING) == -1)
		return -1;

	sys_inb(OUT_BUF, &byte); //Make sure nothing stays in OUT_BUF

	if(mouse_unsubscribe_int() == -1 || timer_unsubscribe_int() == -1)
		return -1;
	else
		return 0;
}

int test_config(void) {
    /* To be completed ... */
}

int test_gesture(short length) {
    /* To be completed ... */
}
