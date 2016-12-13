#pragma once

#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "i8254.h"
#include "timer.h"
#include "utils.h"
#include "video_gr.h"
#include "bitmap.h"

typedef enum state_t {INIT,DRAW,COMP} state;
typedef enum event_t {RUP,RDOWN,MOVE} event;
typedef enum rbutton_t {ISUP,ISDOWN} rbstate;
typedef enum lbutton_t {RELEASED,PRESSED} lbstate;

typedef struct{
	lbstate left_button_state;
	int left_button_was_released; //non-zero when lbstate is RELEASED and previous was PRESSED
	Bitmap* crosshair;
	long packet_byte; //packets' bytes read from OUT_BUF
	unsigned int byteID; //store a packet byte's position in the array
	unsigned int number_of_packets; //try to fix this crap later
	unsigned char packet[3];
}Mouse;

int mouse_subscribe_int();
int mouse_unsubscribe_int();
unsigned long mouse_read_code();
unsigned long mouse_write_code(long destination, unsigned char cmd);
int mouse_write_byte(unsigned char byte);
void mouse_event_handler(state *st, event evt, short *y_variation, short desired_length);

/*-----------------------------*/

Mouse* create_mouse(); //creates new mouse
void draw_mouse(Mouse* mouse, char* buffer); //draw 'mouse' to 'buffer'
void update_mouse(Mouse* mouse); //called when full packet is received, updates position and lbutton state
void delete_mouse(Mouse* mouse);
void mouse_print_packet(unsigned char packet[]);
