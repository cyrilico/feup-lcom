#pragma once

int kbd_subscribe_int();
int kbd_unsubscribe_int();
unsigned long kbd_read_code();
unsigned long kbd_write_code(unsigned char cmd);
void kbd_print_code(unsigned long code);

typedef struct{
	unsigned long scancode;
	unsigned long scancode_aux; //In case of two byte scancodes
	int read_again; //In case of two byte scancodes. Also serves as control (if set to 1, means that scancode does not hold a ready to be processed code)
}Keyboard;

Keyboard* create_keyboard();
void read_scancode(Keyboard* keyboard);
int full_scancode_received(Keyboard* keyboard); //returns 1 if yes (if read_again == 0), 0 otherwise
int key_detected(Keyboard* keyboard, unsigned long key); //returns 1 if 'key' was pressed, 0 otherwise
void delete_keyboard(Keyboard* keyboard);
