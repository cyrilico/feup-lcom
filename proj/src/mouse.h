#pragma once

typedef enum state_t {INIT,DRAW,COMP} state;
typedef enum event_t {RUP,RDOWN,MOVE} event;
typedef enum rbutton_t {ISUP,ISDOWN} rbstate;

int mouse_subscribe_int();
int mouse_unsubscribe_int();
unsigned long mouse_read_code();
unsigned long mouse_write_code(long destination, unsigned char cmd);
int mouse_write_byte(unsigned char byte);
void mouse_event_handler(state *st, event evt, short *y_variation, short desired_length);
