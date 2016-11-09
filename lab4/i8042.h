#ifndef _I8042_H_
#define _I8042_H_

#define IRQ_KBD 1
#define BIT(n) (0x01 << (n))
#define DELAY_US 20000
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60
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

/*New macros created during lab4 (will merge when all functions are implemented; until then it's easier to stay like this if something needs to be changed)*/
#define IRQ_MOUSE 12
#define TWOSCOMPLEMENT(N) (~N+1)
#define WRITE_BYTE_MOUSE 0xD4
#define ENABLE_MOUSE_DATA_REPORTING 0xF4
#define DISABLE_MOUSE_DATA_REPORTING 0xF5
#define GET_MOUSE_CONFIG 0xE9
#define RIGHT_BUTTON_PRESSED BIT(1)

#endif
