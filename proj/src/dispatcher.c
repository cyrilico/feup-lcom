#include "dispatcher.h"

Dispatcher* create_dispatcher(char* db_menu, char* db_game) {
	Dispatcher* dispatcher = (Dispatcher*)(malloc(sizeof(Dispatcher)));

	Menu* menu = create_menu(db_menu);

	dispatcher->state = MAIN_MENU;
	return dispatcher;
}

void update_dispatcher_menu(Dispatcher* dispatcher) {

	int r;
	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	long byte; //Auxiliar variable that will store each byte read

	if(mouse_write_byte(ENABLE_MOUSE_DATA_REPORTING) == -1)
		return;

	while(dispatcher->menu->state != DONE) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & dispatcher->menu->irq_mouse) { /* subscribed interrupt */
					sys_inb(OUT_BUF, &byte);
					if(counter == 0){
						if(byte & BIT(3)) //Valid first byte (or at least we hope so)
							dispatcher->menu->mouse->packet[counter++] = (unsigned char)byte;
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						dispatcher->menu->mouse->packet[counter++] = (unsigned char)byte;
						printf("Increment counter\n");
						if(counter > 2){
							mouse_print_packet(dispatcher->menu->mouse->packet);
							counter = 0;
							if(++number_of_packets != 1){
								update_menu(dispatcher->menu);
								draw_menu(dispatcher->menu);
							}
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

	if(dispatcher->menu->state == DONE)
		dispatcher->state = EXIT_PROGRAM;

}

void delete_dispatcher(Dispatcher* dispatcher) {
	delete_menu(dispatcher->menu);
	free(dispatcher);
}

