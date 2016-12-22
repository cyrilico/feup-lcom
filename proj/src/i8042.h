#pragma once

#define IRQ_KBD 1
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60
#define GET_MSB 0xFF00
#define OBF 0x1 //BIT(0)
#define IBF 0x2 //BIT(1)
#define TIMEOUT 0x40 //BIT(6)
#define PARITY 0x80 //BIT(7)
#define OK 0
#define NTRIES 5
#define TWO_BYTE_SCANCODE 0xE0
#define PROCESS_LEDS 0xED
#define ACK 0xFA
#define RESEND 0xFE
#define ERROR 0xFC
#define DISABLE_SCANCODES 0xF5
#define ENABLE_SCANCODES 0xF4
#define ESC_BREAK 0x81
#define DELAY_US 20000

#define A_BREAK 0x9E
#define B_BREAK 0xB0
#define C_BREAK 0xAE
#define D_BREAK 0xA0
#define E_BREAK 0x92
#define F_BREAK 0xA1
#define G_BREAK 0xA2
#define H_BREAK 0xA3
#define I_BREAK 0x97
#define J_BREAK 0xA4
#define K_BREAK 0xA5
#define L_BREAK 0xA6
#define M_BREAK 0xB2
#define N_BREAK 0xB1
#define O_BREAK 0x98
#define P_BREAK 0x99
#define Q_BREAK 0x90
#define R_BREAK 0x93
#define S_BREAK 0x9F
#define T_BREAK 0x94
#define U_BREAK 0x96
#define V_BREAK 0xAF
#define W_BREAK 0x91
#define X_BREAK 0xAD
#define Y_BREAK 0x95
#define Z_BREAK 0xAC
#define NOT_VALID ""

#define IRQ_MOUSE 12
#define MAX_X_TOLERANCE 1
#define MAX_Y_TOLERANCE 1
#define WRITE_BYTE_MOUSE 0xD4
#define ENABLE_MOUSE_DATA_REPORTING 0xF4
#define DISABLE_MOUSE_DATA_REPORTING 0xF5
#define GET_MOUSE_CONFIG 0xE9
