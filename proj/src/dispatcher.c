#include "dispatcher.h"

Dispatcher* create_dispatcher() {
	Dispatcher* dispatcher = (Dispatcher*)(malloc(sizeof(Dispatcher)));

	//dispatcher->menu = create_menu();
	dispatcher->game = create_game();
	dispatcher->state = GAME;
	return dispatcher;
}

void process_main_menu(Dispatcher* dispatcher) {
	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	long byte; //Auxiliar variable that will store each byte read

	mouse_write_byte(ENABLE_MOUSE_DATA_REPORTING);

	while(dispatcher->menu->state != DONE) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0 )
			continue;
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if(msg.NOTIFY_ARG & dispatcher->menu->irq_mouse) {
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
							if(++number_of_packets != 1)
								update_menu(dispatcher->menu);
						}
					}
				}
				else if(msg.NOTIFY_ARG & dispatcher->menu->irq_timer)
					draw_menu(dispatcher->menu);
				break;
			default:
				break;
			}
		}
	}

	mouse_write_byte(DISABLE_MOUSE_DATA_REPORTING);
	if(dispatcher->menu->state == DONE)
		dispatcher->state = EXIT_PROGRAM;

}

void process_game(Dispatcher* dispatcher) {
	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	long byte; //Auxiliar variable that will store each byte read

	mouse_write_byte(ENABLE_MOUSE_DATA_REPORTING);

	while(dispatcher->game->state != GDONE) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0 )
			continue;
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if(msg.NOTIFY_ARG & dispatcher->game->irq_mouse) {
					sys_inb(OUT_BUF, &byte);
					if(counter == 0){
						if(byte & BIT(3)) //Valid first byte (or at least we hope so)
							dispatcher->game->mouse->packet[counter++] = (unsigned char)byte;
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						dispatcher->game->mouse->packet[counter++] = (unsigned char)byte;
						printf("Increment counter\n");
						if(counter > 2){
							mouse_print_packet(dispatcher->game->mouse->packet);
							counter = 0;
							if(++number_of_packets != 1)
								update_game(dispatcher->game);
						}
					}
				}
				else if(msg.NOTIFY_ARG & dispatcher->game->irq_timer)
					draw_game(dispatcher->game);
				break;
			default:
				break;
			}
		}
	}

	mouse_write_byte(DISABLE_MOUSE_DATA_REPORTING);
	if(dispatcher->game->state == GDONE)
		dispatcher->state = EXIT_PROGRAM;

}

void delete_dispatcher(Dispatcher* dispatcher) {
	delete_menu(dispatcher->menu);
	free(dispatcher);
}

