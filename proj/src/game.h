#pragma once

#include "bitmap.h"
#include "mouse.h"
#include "keyboard.h"
#include "timer.h"
#include "video_gr.h"
#include "utils.h"

typedef struct{
	int x;
	int y;
	int width;
	int height;
	int color;
	int lives;
	int colors[4];
}Obstacle;

Obstacle* create_obstacle(int x, int y, int width, int height);
void update_obstacle(Obstacle* obstacle);
void draw_obstacle(Obstacle* obstacle, char* buffer);

typedef enum gamestate_t {GNOT_DONE, GDONE} gamestate;

typedef struct{

	Obstacle* obstacle;
	Mouse* mouse;
	Bitmap* background;
	char* double_buffer;
	gamestate state;
}Game;

Game* create_game();
void update_game(Game* game);
void draw_game(Game* game);
