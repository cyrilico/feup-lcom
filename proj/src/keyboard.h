#pragma once

int kbd_subscribe_int();
int kbd_unsubscribe_int();
unsigned long kbd_read_code();
unsigned long kbd_write_code(unsigned char cmd);
void kbd_print_code(unsigned long code);
int kbd_scan_loop();
int kbd_timed_scan_loop(unsigned short n);
int kbd_send_led_cmd();
int kbd_process_leds(unsigned long led_sequence);
unsigned long kbd_change_led_sequence(unsigned long led);
int kbd_leds_loop(unsigned short n, unsigned short *leds);

typedef struct{
	unsigned long scancode;
	unsigned long scancode_aux; //In case of two byte scancodes
	int read_again; //In case of two byte scancodes. Also serves as control (if set to 1, means that scancode does not hold a ready to be processed code)
}Keyboard;

Keyboard* create_keyboard();
void read_scancode(Keyboard* keyboard);
void delete_keyboard(Keyboard* keyboard);
