#ifndef __KEYBOARD_H
#define __KEYBOARD_H

int kbd_scan_loop(unsigned short c_or_asm);
int kbd_subscribe_int();
int kbd_unsubscribe_int();
unsigned long kbd_read_code();
unsigned long kbd_write_code(unsigned char cmd);
void kbd_print_code(unsigned long code);
int kbd_timed_scan_loop(unsigned short n);
int kbd_send_led_cmd();
int kbd_process_leds(unsigned long led_sequence);
unsigned long kbd_change_led_sequence(unsigned long led);
int kbd_leds_loop(unsigned short n, unsigned short *leds);
#endif
