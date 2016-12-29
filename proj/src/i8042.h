#pragma once

/** @defgroup i8042 i8042
 * @ingroup controller_macros
 * @brief Constants for programming the i8042 controller. Needs to be completed.
 * @{
 *
 */

 /* IRQs */

#define IRQ_KBD     1 /**< @brief KBD IRQ line */
#define IRQ_MOUSE   12 /**< @brief Mouse IRQ line */

#define STAT_REG        0x64    /**< @brief Status Register for KBD commands */
#define KBC_CMD_REG     0x64    /**< @brief Register for KBC commands */
#define IN_BUF          0x60    /**< @brief KBC Input Buffer */
#define OUT_BUF         0x60    /**< @brief KBC Output Buffer */

/* Mouse Commands */
#define WRITE_BYTE_MOUSE                0xD4    /**< @brief Write byte to mouse command */
#define ENABLE_MOUSE_DATA_REPORTING     0xF4    /**< @brief Enable mouse's data reporting - in stream mode only */
#define DISABLE_MOUSE_DATA_REPORTING    0xF5    /**< @brief Disable mouse's data reporting - in stream mode only */

/* KBD Commands */
#define DISABLE_SCANCODES   0xF5    /**< @brief Disable KBD - set default values and stop scanning */
#define ENABLE_SCANCODES    0xF4    /**< @brief Clear buffer and enable KBD */

/* Status Register's responses */
#define TIMEOUT             0x40    /**< @brief Timeout error - invalid data - BIT(6) */
#define PARITY              0x80    /**< @brief Parity error - invalid data - BIT(7) */
#define OBF                 0x1     /**< @brief Output buffer full - BIT(0) */
#define IBF                 0x2     /**< @brief Input buffer full - BIT(1) */

/* Keyboard Control */

#define NTRIES              5    /**< @brief Number of tries in_buf or out_buf are read before aborting */
#define TWO_BYTE_SCANCODE   0xE0    /**< @brief First byte of two byte scancode (usually) */

/* C@KBC Responses */
#define ACK                 0xFA    /**< @brief Means sucessful command */
#define RESEND              0xFE    /**< @brief Need to resend current byte */
#define ERROR               0xFC    /**< @brief Means error in current command - the entire sequence should be restarted */

#define DELAY_US 20000 /**< @brief Delay time between i8042 out_buf accesses (in ms) */

/* Key Breakcodes */

#define A_BREAK     0x9E
#define B_BREAK     0xB0
#define C_BREAK     0xAE
#define D_BREAK     0xA0
#define E_BREAK     0x92
#define F_BREAK     0xA1
#define G_BREAK     0xA2
#define H_BREAK     0xA3
#define I_BREAK     0x97
#define J_BREAK     0xA4
#define K_BREAK     0xA5
#define L_BREAK     0xA6
#define M_BREAK     0xB2
#define N_BREAK     0xB1
#define O_BREAK     0x98
#define P_BREAK     0x99
#define Q_BREAK     0x90
#define R_BREAK     0x93
#define S_BREAK     0x9F
#define T_BREAK     0x94
#define U_BREAK     0x96
#define V_BREAK     0xAF
#define W_BREAK     0x91
#define X_BREAK     0xAD
#define Y_BREAK     0x95
#define Z_BREAK     0xAC
#define SPACE_BREAK     0xB9
#define ENTER_BREAK     0x9C
#define BACKSPACE_BREAK     0x8E
#define ESC_BREAK   0x81
#define NOT_VALID 0

/**@}*/ // end of i8042
