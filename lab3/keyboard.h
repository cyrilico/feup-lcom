#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#define BIT(n) (0x01<<(n))
#define IRQ_KBD 1
#define DELAY_US 20000
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60
#define GET_LSB 0x00FF
#define GET_MSB 0xFF00
#define ESC_BREAK 0x81
#define OBF BIT(0)
#define IBF BIT(1)
#define TIMEOUT BIT(6)
#define PARITY BIT(7)
#define OK 0
#define NTRIES 5
#define TWO_BYTE_SCANCODE 0xE0
#define PROCESS_LEDS 0xED
#define ACK 0xFA
#define RESEND 0xFE
#define ERROR 0xFC
#define DISABLE_SCANCODES 0xF5
#define ENABLE_SCANCODES 0xF4

int kbd_scan_loop();
int kbd_subscribe_int(int *hookid);
int kbd_unsubscribe_int(int *hookid);
unsigned long kbd_read_code();
unsigned long kbd_write_code(unsigned char cmd);
void kbd_print_code(unsigned long code);
int kbd_timed_scan_loop(unsigned short n);
int kbd_send_led_cmd();
int kbd_process_leds(unsigned long led_sequence);
unsigned long kbd_change_led_sequence(unsigned long led);
int kbd_leds_loop(unsigned short n, unsigned short *leds);
#endif
