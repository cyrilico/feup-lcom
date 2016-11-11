#ifndef __MOUSE_H
#define __MOUSE_H

#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "i8254.h"
#include "timer.h"

typedef enum state_t {INIT,DRAW,COMP} state;
typedef enum event_t {RUP,RDOWN,MOVE} event;
typedef enum rbutton_t {ISUP,ISDOWN} rbstate;

int mouse_subscribe_int();
int mouse_unsubscribe_int();
unsigned long mouse_read_code();
unsigned long mouse_write_code(long destination, unsigned char cmd);
void mouse_event_handler(state *st, event evt, short *y_variation, short desired_length);

#endif
