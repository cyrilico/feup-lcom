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

	while(menu->state == NOT_DONE) {
		if (driver_receive(ANY, &(dispatcher->msg), &(dispatcher->ipc_status)) != 0 )
			continue;
		if (is_ipc_notify(dispatcher->ipc_status)) {
			switch (_ENDPOINT_P((dispatcher->msg).m_source)) {
			case HARDWARE:
				if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_mouse) {
					read_packet_byte(menu->mouse);
					if(full_packet_received(menu->mouse));
						update_menu(menu,MOUSE_UPDATE);
				}
				else if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_kbd){
					read_scancode(menu->keyboard);
					if(full_scancode_received(menu->keyboard))
						update_menu(menu,KBD_UPDATE);
				}
				else if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_timer)
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
	unsigned short counter = 0;

	while(game->state != GAME_OVER) {
		if (driver_receive(ANY, &(dispatcher->msg), &(dispatcher->ipc_status)) != 0 )
			continue;
		if (is_ipc_notify(dispatcher->ipc_status)) {
			switch (_ENDPOINT_P((dispatcher->msg).m_source)) {
			case HARDWARE:
				if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_mouse)
					read_packet_byte(game->mouse);
				else if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_kbd){
					read_scancode(game->keyboard);
					/* TO DO: Turn 'if' content into a function (ex: scancode_received(scancode). return 1 if yes, 0 if no */
					if(key_detected(game->keyboard, ESC_BREAK))
						game->state = GAME_OVER;
					else if(key_detected(game->keyboard, A_BREAK) && game->player->number_of_bullets > 0){ /*TO DO: Layer this piece of code */
						if(add_bullet_shot(game,game->player->bitmap->x,game->player->bitmap->y) == 1)
							game->player->number_of_bullets--;
					}
				}
				else if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_timer){
					counter = (counter+1)%2;
					update_game(game);
					if(full_packet_received(game->mouse)){
						reset_packet_state(game->mouse);
						update_player_mouse(game->player, game->mouse, game->secondary_buffer);
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

