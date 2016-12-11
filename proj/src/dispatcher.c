#include "dispatcher.h"

Dispatcher* create_dispatcher() {
	Dispatcher* dispatcher = (Dispatcher*)(malloc(sizeof(Dispatcher)));

	dispatcher->irq_timer = timer_subscribe_int();
	dispatcher->irq_kbd = kbd_subscribe_int();
	dispatcher->irq_mouse = mouse_subscribe_int();
	dispatcher->state = MAIN_MENU;
	return dispatcher;
}

void process_main_menu(Dispatcher* dispatcher) {

	Menu* menu = create_menu();

	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	long byte; //Auxiliar variable that will store each byte read

	while(menu->state == NOT_DONE) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0 )
			continue;
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if(msg.NOTIFY_ARG & dispatcher->irq_mouse) {
					sys_inb(OUT_BUF, &byte);
					if(counter == 0){
						if(byte & BIT(3)) //Valid first byte (or at least we hope so)
							menu->mouse->packet[counter++] = (unsigned char)byte;
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						menu->mouse->packet[counter++] = (unsigned char)byte;
						printf("Increment counter\n");
						if(counter > 2){
							mouse_print_packet(menu->mouse->packet);
							counter = 0;
							if(++number_of_packets != 1)
								update_menu(menu);
						}
					}
				}
				else if(msg.NOTIFY_ARG & dispatcher->irq_timer)
					draw_menu(menu);
				break;
			default:
				break;
			}
		}
	}

	//Change menu state
	switch(menu->state){
	case PLAY_CHOSEN:
		dispatcher->state = GAME;
		break;
	case EXIT_CHOSEN:
		dispatcher->state = EXIT_PROGRAM;
		break;
	default:
		break;
	}

	delete_menu(menu);
}

void process_game(Dispatcher* dispatcher) {

	Game* game = create_game();

	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	long byte; //Auxiliar variable that will store each byte read

	while(game->state != GAME_OVER) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0 )
			continue;
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if(msg.NOTIFY_ARG & dispatcher->irq_mouse) {
					sys_inb(OUT_BUF, &byte);
					if(game->mouse->byteID == 0){
						if(byte & BIT(3)) //Valid first byte (or at least we hope so)
							game->mouse->packet[game->mouse->byteID++] = (unsigned char)byte;
						else
							continue;
					}
					else{
						game->mouse->packet[game->mouse->byteID++] = (unsigned char)byte;
						if(game->mouse->byteID == 3){
							mouse_print_packet(game->mouse->packet);
							game->mouse->byteID = 0;
							if(++number_of_packets != 1)
								update_game(game);
						}
					}
				}
				else if(msg.NOTIFY_ARG & dispatcher->irq_timer)
					draw_game(game);
				else if(msg.NOTIFY_ARG & dispatcher->irq_kbd)
					sys_inb(OUT_BUF, &byte);
				break;
			default:
				break;
			}
		}
	}
	dispatcher->state = MAIN_MENU;
}

void delete_dispatcher(Dispatcher* dispatcher) {
	timer_unsubscribe_int();
	kbd_unsubscribe_int();
	mouse_unsubscribe_int();
	free(dispatcher);
}

