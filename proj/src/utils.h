#pragma once

#include <string.h>
#include <stdio.h>
#include "bitmap.h"
#include "video_gr.h"

#define BIT(n) (0x01<<(n))
#define NBYTE(n) (0xff<<(8*n))
#define TWOSCOMPLEMENT(N) (short)(0xff00 | N)
#define ABS_VALUE(X) (X < 0 ? -X : X)
#define OK 0 //Sometimes is not defined for some reason

#define GAME_NUMBER_WIDTH 16
#define SCORE_BITMAP_WIDTH 30
#define SCORE_BITMAP_HEIGHT 30 //Higher value? if it's 30 score in line x can be too close to score in line x-1

#define BLACK 0
#define PINK 0xF81F
#define RED 0xF800
#define BLUE 0x001F
#define GREEN 0x07E0

const char* fullPath(const char* filename);
void draw_game_number(int number, int x, int y, char* buffer); //draws 16x24 numbers bitmaps (used in game)
char* number_to_string(int number); //1 produces "one", 2 produces "two", ...
void draw_score_number(int number, int x, int y, char* buffer); //draws 30x30 numbers bitmaps (used in final score screen)
void draw_colon(int x, int y, char* buffer); //draws char ':'
void draw_slash(int x, int y, char* buffer); //draws char '/
void draw_letter(char letter, int x, int y, char* buffer);
int start_graphic_mode();
int exit_graphic_mode();


