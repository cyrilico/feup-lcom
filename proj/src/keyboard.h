#pragma once

#include "i8042.h"

int kbd_subscribe_int();
int kbd_unsubscribe_int();
char scancode_to_letter(unsigned long code); //transforms breakcodes into the respective letter

typedef enum scancodestate_t {READ_AGAIN, FULLY_READ} scancodestate;

typedef struct{
	unsigned long scancode;
	unsigned long scancode_aux; //In case of two byte scancodes
	scancodestate codestatus; //In case of two byte scancodes. Also serves as control
}Keyboard;

Keyboard* create_keyboard();
void read_scancode(Keyboard* keyboard);
int full_scancode_received(Keyboard* keyboard); //returns 1 if yes (if read_again == 0), 0 otherwise
int key_detected(Keyboard* keyboard, unsigned long key); //returns 1 if 'key' was pressed, 0 otherwise
void delete_keyboard(Keyboard* keyboard);
