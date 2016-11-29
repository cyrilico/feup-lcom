#ifndef __PS2_H_
#define __PS2_H_

#define IRQ_MOUSE 12
#define TWOSCOMPLEMENT(N) (short)(0xff00 | N)
#define ABS_VALUE(X) (X < 0 ? -X : X)
#define MAX_X_TOLERANCE 1
#define MAX_Y_TOLERANCE 1
#define WRITE_BYTE_MOUSE 0xD4
#define ENABLE_MOUSE_DATA_REPORTING 0xF4
#define DISABLE_MOUSE_DATA_REPORTING 0xF5
#define GET_MOUSE_CONFIG 0xE9

#endif
