#include <minix/drivers.h>
#include "game.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "video.h"
#include "i8042.h"
#include "bitmap.h"
#include "keyboard.h"
#include "mouse.h"


Game* start_game() {
	Game* game = (Game*) malloc(sizeof(Game));

	//Subscribe keyboard
    int irq_set = kbd_subscribe_int();
    if(irq_set == -1)
        return (Game*)-1;

    game->IRQ_SET_KBD = irq_set;

    game->done = 0;
    game->draw = 1;
    game->scancode = 0;
    return game;
}

int update_game(Game* game) {

    int ipc_status;
    message msg;

    int read_again = 0;
    unsigned long code = 0;
    unsigned long code_aux = 0;

    if ( driver_receive(ANY, &msg, &ipc_status) != 0 )
    	return;

    if (is_ipc_notify(ipc_status)) { /* received notification */
    	switch (_ENDPOINT_P(msg.m_source)) {
    	case HARDWARE: /* hardware interrupt notification */
    		if (msg.NOTIFY_ARG & game->IRQ_SET_KBD) { /* subscribed interrupt */
    			code = kbd_read_code();
    			if(code == -1)
    				return -1;
    			else if(read_again == 1){
    				code = code << 8;
    				code |= code_aux;
    				read_again = 0;
    			}
    			else if(code == TWO_BYTE_SCANCODE){
    				code_aux = code;
    				read_again = 1;
    			}
    		}
    		break;
    	default:
    		break; /* no other notifications expected: do nothing */
    	}
    } else { /* received a standard message, not a notification */
    	/* no standard messages expected: do nothing */
    }
    if(!read_again){
    	game->scancode = code;
    	if(code == ESC_MAKE)
    		game->done = 1;
    	else
    		game->draw = 1;
    }
    tickdelay(micros_to_ticks(DELAY_US));
}


int exit_game(Game* game) {

	//Unsubscribe keyboard interruptions
	int kbd_unsubscribe_int();
}
