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

typedef enum lbutton_t {RELEASED,PRESSED} lbstate;
typedef enum packetstate_t {RECEIVING, RECEIVED} packetstate;

typedef struct{
	lbstate left_button_state;
	int left_button_was_released; //non-zero when lbstate is RELEASED and previous was PRESSED
	Bitmap* crosshair;
	long packet_byte; //packets' bytes read from OUT_BUF
	unsigned int byteID; //store a packet byte's position in the array
	unsigned char packet[3];
	packetstate packet_state;
}Mouse;

int mouse_subscribe_int();
int mouse_unsubscribe_int();
unsigned long mouse_read_code();
unsigned long mouse_write_code(long destination, unsigned char cmd);
int mouse_write_byte(unsigned char byte);

/*-----------------------------*/

Mouse* create_mouse(); //creates new mouse
Mouse* create_game_mouse(); //creates new mouse for in game use
void draw_mouse(Mouse* mouse, char* buffer); //draw 'mouse' to 'buffer'
void read_packet_byte(Mouse* mouse);
void update_mouse(Mouse* mouse); //called when full packet is received, updates position and lbutton state
int full_packet_received(Mouse* mouse); //return 1 if packet_state == RECEIVED, 0 otherwise
void reset_packet_state(Mouse* mouse); //resets packet_state to RECEIVING
void delete_mouse(Mouse* mouse);
void delete_game_mouse(Mouse* mouse);
void mouse_print_packet(unsigned char packet[]);
