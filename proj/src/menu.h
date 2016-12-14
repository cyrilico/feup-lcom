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

#define KBD_UPDATE 0
#define MOUSE_UPDATE 1

typedef enum menustate_t {NOT_DONE, PLAY_CHOSEN, EXIT_CHOSEN} menustate;

typedef struct{
	Button* play_button;
	Button* exit_button;
	Mouse* mouse;
	Keyboard* keyboard;
	Bitmap* background;
	char* secondary_buffer;

	menustate state;
}Menu;

Menu* create_menu();
void draw_menu(Menu* menu);
void update_menu(Menu* menu, int kbd_or_mouse); //0 if kbd caused the update, 1 if mouse caused the update
void delete_menu(Menu* menu);
