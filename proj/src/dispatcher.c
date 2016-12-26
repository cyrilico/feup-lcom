#include "dispatcher.h"

Dispatcher* create_dispatcher() {
	Dispatcher* dispatcher = (Dispatcher*)(malloc(sizeof(Dispatcher)));

	dispatcher->irq_timer = timer_subscribe_int();
	dispatcher->irq_kbd = kbd_subscribe_int();
	dispatcher->irq_mouse = mouse_subscribe_int();
	dispatcher->irq_rtc = rtc_subscribe_int();
	dispatcher->state = MAIN_MENU;
	return dispatcher;
}

/*
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
} */

/*
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
					if(key_detected(game->keyboard, ESC_BREAK))
						game->state = GAME_OVER;
					else if(key_detected(game->keyboard, A_BREAK) && player_has_bullets(game->player)){ //TO DO: Layer this piece of code
						if(add_bullet_shot(game,game->player->bitmap->x,game->player->bitmap->y) == 1)
							update_number_of_bullets(game->player);
					}
				}
				else if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_timer){
					update_draw_state(game);
					update_game(game);
					if(full_packet_received(game->mouse)){
						reset_packet_state(game->mouse);
						update_player_mouse(game->player, game->mouse, game->secondary_buffer);
					}
					if(game->drawstate == DRAW)
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
*/

void interrupt_handler(Dispatcher* dispatcher) {

	Menu* menu = NULL;
	Game* game = NULL;

	if(dispatcher->state == MAIN_MENU) {
		printf("Creating menu\n");
		menu = create_menu();
		printf("Created menu\n");
	}
	else {//dispatcher->state == GAME
		printf("Creating game\n");
		game = create_game();
		printf("Created game\n");
	}

	dispatcherstate initial_state = dispatcher->state;

	while(initial_state == dispatcher->state) {
		if (driver_receive(ANY, &(dispatcher->msg), &(dispatcher->ipc_status)) != 0 )
			continue;
		if (is_ipc_notify(dispatcher->ipc_status)) {
			switch (_ENDPOINT_P((dispatcher->msg).m_source)) {
			case HARDWARE:
				if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_mouse) {
					if(dispatcher->state == MAIN_MENU) {
						read_packet_byte(menu->mouse);
						if(full_packet_received(menu->mouse))
							update_menu(menu,MOUSE_UPDATE);
					}
					else if(dispatcher->state == GAME){
						read_packet_byte(game->mouse);
						if(full_packet_received(game->mouse)){
							reset_packet_state(game->mouse);
							if(game->state == GAME_RUNNING)
								update_player_mouse(game->player, game->mouse, game->secondary_buffer);
						}
					}
				}
				else if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_kbd){
					if(dispatcher->state == MAIN_MENU) {
						read_scancode(menu->keyboard);
						if(full_scancode_received(menu->keyboard))
							update_menu(menu,KBD_UPDATE);
					}
					else { //dispatcher->state == GAME
						read_scancode(game->keyboard);
						if(game->state == GAME_RUNNING) {
							if(key_detected(game->keyboard, A_BREAK) && player_has_bullets(game->player)){ /* Shooting key: A */
								if(add_bullet_shot(game,game->player->bitmap->x,game->player->bitmap->y) == 1) /* If functions returns 1, sucessfully shot */
									update_number_of_bullets(game->player);
							}
							else if(key_detected(game->keyboard, SPACE_BREAK))
								game->state = GAME_PAUSED;
						}
						else if(game->state == GAME_PAUSED){
							if(key_detected(game->keyboard, SPACE_BREAK))
								game->state = GAME_RUNNING;
						}
						else { //game->state == GAME_SCORE
							if(full_scancode_received(game->keyboard))
								update_game_score(game);
						}
					}
				}
				else if((dispatcher->msg).NOTIFY_ARG & dispatcher->irq_timer){

					if(dispatcher->state == MAIN_MENU)
						draw_menu(menu);
					else { //dispatcher->state == GAME
						update_draw_state(game);
						if(game->state == GAME_RUNNING)
							update_game_running(game);
						if(game->drawstate == DRAW)
							draw_game(game);
					}
				}
				break;
			default:
				break;
			}
		}

		//Check for dispatcher state changes
		state_handler(dispatcher, menu, game);
	}

}

void delete_dispatcher(Dispatcher* dispatcher) {
		timer_unsubscribe_int();
		kbd_unsubscribe_int();
		mouse_unsubscribe_int();
		rtc_unsubscribe_int();

		free(dispatcher);
}

void state_handler(Dispatcher* dispatcher, Menu* menu, Game* game) {
	if(menu != NULL) {
		if(dispatcher->state == MAIN_MENU && menu->state != NOT_DONE) {
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
			printf("Deleting menu\n");
			delete_menu(menu);
			menu = NULL;
			printf("Menu deleted\n");
		}
	}
	else if(game != NULL) {
		if(dispatcher->state == GAME && game->state == GAME_OVER) {
			dispatcher->state = MAIN_MENU;
			printf("Deleting game\n");
			delete_game(game);
			game = NULL;
			printf("Game deleted\n");
		}
	}
}
