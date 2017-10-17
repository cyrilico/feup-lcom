#ifndef __KEYBOARD_H
#define __KEYBOARD_H

int kbd_scan_loop(unsigned short c_or_asm);
int kbd_subscribe_int(int *hookid);
int kbd_unsubscribe_int(int *hookid);
unsigned long kbd_read_code();
unsigned long kbd_write_code(unsigned char cmd);
unsigned long kbd_write_code_arg(unsigned char reg, unsigned char arg);
unsigned long kbd_write_code_reg(unsigned char reg, unsigned char cmd);
void kbd_print_code(unsigned long code);
int kbd_timed_scan_loop(unsigned short n);
int kbd_send_led_cmd();
int kbd_process_leds(unsigned long led_sequence);
unsigned long kbd_change_led_sequence(unsigned long led);
int kbd_leds_loop(unsigned short n, unsigned short *leds);
int kbd_poll();
int kbd_reset_status();
#endif
