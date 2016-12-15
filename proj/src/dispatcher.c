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

	while(menu->state == NOT_DONE) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0 )
			continue;
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if(msg.NOTIFY_ARG & dispatcher->irq_mouse) {
					sys_inb(OUT_BUF, &(menu->mouse->packet_byte));
					if(menu->mouse->byteID == 0){
						if(menu->mouse->packet_byte & BIT(3)) //Valid first byte (or at least we hope so)
							menu->mouse->packet[menu->mouse->byteID++] = (unsigned char)(menu->mouse->packet_byte);
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						menu->mouse->packet[menu->mouse->byteID++] = (unsigned char)(menu->mouse->packet_byte);
						if(menu->mouse->byteID == 3){
							mouse_print_packet(menu->mouse->packet);
							menu->mouse->byteID = 0;
							if(++(menu->mouse->number_of_packets) != 1){
								update_mouse(menu->mouse);
								update_menu(menu,MOUSE_UPDATE);
							}
						}
					}
				}
				else if(msg.NOTIFY_ARG & dispatcher->irq_kbd){
					read_scancode(menu->keyboard);
					if(!menu->keyboard->read_again) //Full scancode processed, analyze its consequences
						update_menu(menu,KBD_UPDATE);
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

	unsigned short counter = 0;
	int update = 0;

	while(game->state != GAME_OVER) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0 )
			continue;
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if(msg.NOTIFY_ARG & dispatcher->irq_mouse){
					sys_inb(OUT_BUF, &(game->mouse->packet_byte));
					if(game->mouse->byteID == 0){
						if(game->mouse->packet_byte & BIT(3)) //Valid first byte (or at least we hope so)
							game->mouse->packet[game->mouse->byteID++] = (unsigned char)(game->mouse->packet_byte);
						else
							continue;
					}
					else{
						game->mouse->packet[game->mouse->byteID++] = (unsigned char)(game->mouse->packet_byte);
						if(game->mouse->byteID == 3){
							mouse_print_packet(game->mouse->packet);
							game->mouse->byteID = 0;
							if(++(game->mouse->number_of_packets) != 1)
								update = 1;
						}
					}
				}
				else if(msg.NOTIFY_ARG & dispatcher->irq_kbd){
					read_scancode(game->keyboard);
					if(!game->keyboard->read_again && game->keyboard->scancode == ESC_BREAK) //Full scancode processed, analyze its consequences
						game->state = GAME_OVER;
					else if(!game->keyboard->read_again && game->keyboard->scancode == A_BREAK) //Shoot bullet
						remove_bullet_shot(game);
				}
				else if(msg.NOTIFY_ARG & dispatcher->irq_timer){
					counter = (counter+1)%2;
					update_game(game);
					if(update){
						update = 0;
						update_mouse(game->mouse);
						update_player_mouse(game->player, game->mouse);
					}
					if(counter)
						draw_game(game);
				}
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

