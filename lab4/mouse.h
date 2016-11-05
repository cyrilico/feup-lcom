#ifndef __MOUSE_H
#define __MOUSE_H

int mouse_subscribe_int(int *hookid);
int mouse_unsubscribe_int(int *hookid);
unsigned long mouse_read_code();
