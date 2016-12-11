#pragma once

#include "bitmap.h"
#include "mouse.h"
#include "keyboard.h"
#include "timer.h"
#include "video_gr.h"
#include "utils.h"

typedef struct{
	unsigned int lives;
	Bitmap* bitmaps[3]; //Different bitmap according to number of lives
}Obstacle;

Obstacle* create_obstacle(int x, int y);
void update_obstacle(Obstacle* obstacle);
void draw_obstacle(Obstacle* obstacle, char* buffer);

typedef enum gamestate_t {GAME_RUNNING, GAME_OVER} gamestate;

typedef struct{
	Obstacle** obstacle;
	Mouse* mouse;
	Bitmap* background;
	Bitmap* player;
	char* secondary_buffer;
	gamestate state;
}Game;

Game* create_game();
void update_game(Game* game);
void draw_game(Game* game);
void delete_game(Game* game);
