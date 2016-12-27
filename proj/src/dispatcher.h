#pragma once

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "menu.h"
#include "game.h"

typedef enum dispatcherstate_t {MAIN_MENU,GAME,EXIT_PROGRAM} dispatcherstate;

typedef struct{
	int irq_timer;
	int irq_kbd;
	int irq_mouse;
	int irq_rtc;

	message msg;
	int ipc_status;
	char* pwd;
	dispatcherstate state;
} Dispatcher;

Dispatcher* create_dispatcher(char* path);
/*
void process_main_menu(Dispatcher* dispatcher);
void process_game(Dispatcher* dispatcher);*/
void delete_dispatcher(Dispatcher* dispatcher);
char* get_pwd();

void interrupt_handler(Dispatcher* dispatcher);

void state_handler(Dispatcher* dispatcher, Menu* menu, Game* game);
