#pragma once

#define PLAY_BUTTON_X 300
#define PLAY_BUTTON_Y 280
#define EXIT_BUTTON_X 300
#define EXIT_BUTTON_Y 345
#define BUTTON_WIDTH 145
#define BUTTON_HEIGHT 50

#include "bitmap.h"
#include "mouse.h"
#include "keyboard.h"
#include "timer.h"
#include "video_gr.h"
#include "utils.h"

//Used for main menu buttons
typedef struct{
	int xi;
	int yi;
	int width;
	int height;
}Button;

Button* create_button(int x, int y, int width, int height);
int mouse_on_button(Button* button, Mouse* mouse);
void delete_button(Button* button);

typedef enum menustate_t {NOT_DONE,DONE} menustate;

typedef struct{
	int irq_timer;
	int irq_keyboard;
	int irq_mouse;

	Button* play_button;
	Button* exit_button;
	Mouse* mouse;
	Bitmap* background;
	char* double_buffer;

	menustate state;
}Menu;

Menu* create_menu(char* dbuffer);
void draw_menu(Menu* menu);
void delete_menu(Menu* menu);
