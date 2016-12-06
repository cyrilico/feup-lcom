#pragma once

#include "menu.h"
#include "game.h"

typedef enum dispatcherstate_t {MAIN_MENU,GAME,EXIT_PROGRAM} dispatcherstate;

typedef struct{
	Menu* menu;
	Game* game;
	dispatcherstate state;
} Dispatcher;

Dispatcher* create_dispatcher();
void process_main_menu(Dispatcher* dispatcher);
void process_game(Dispatcher* dispatcher);
void delete_dispatcher(Dispatcher* dispatcher);


