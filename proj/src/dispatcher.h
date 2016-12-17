#pragma once

#include "menu.h"
#include "game.h"

typedef enum dispatcherstate_t {MAIN_MENU,GAME,EXIT_PROGRAM} dispatcherstate;

typedef struct{
	int irq_timer;
	int irq_kbd;
	int irq_mouse;

	message msg;
	int ipc_status;
	dispatcherstate state;
} Dispatcher;

Dispatcher* create_dispatcher();
void process_main_menu(Dispatcher* dispatcher);
void process_game(Dispatcher* dispatcher);
void delete_dispatcher(Dispatcher* dispatcher);


