#pragma once

#include "menu.h"

typedef enum dispatcherstate_t {MAIN_MENU,GAME,EXIT_PROGRAM} dispatcherstate;

typedef struct{
	Menu* menu;
	//Game* game;
	dispatcherstate state;

} Dispatcher;

Dispatcher* create_dispatcher(char* db_menu, char* db_game);
void update_dispatcher_menu(Dispatcher* dispatcher);

void delete_dispatcher(Dispatcher* dispatcher);


