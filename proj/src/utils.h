#pragma once

#include <string.h>
#include "stdio.h"
#include "video_gr.h"

#define BIT(n) (0x01<<(n))
#define NBYTE(n) (0xff<<(8*n))
#define DELAY_US 20000
//#define TWOSCOMPLEMENT(N) (short)(0xff00 | N)
#define TWOSCOMPLEMENT(N) (short)(0xff00 | N)
#define ABS_VALUE(X) (X < 0 ? -X : X)
#define OK 0 //Sometimes is not defined for some reason

int file_exists(const char* filename);
const char* fullPath(const char* filename);
int rgb(unsigned char r, unsigned char g, unsigned char b);
int start_graphic_mode();
int exit_graphic_mode();


