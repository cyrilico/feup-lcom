#pragma once

#include "bitmap.h"
#include "mouse.h"
#include "keyboard.h"
#include "timer.h"
#include "video_gr.h"
#include "utils.h"

#define OBSTACLE_WIDTH 56
#define OBSTACLE_HEIGHT 56

typedef struct{
	unsigned int lives;
	Bitmap* bitmaps[3]; //Different bitmap according to number of lives
}Obstacle;

Obstacle* create_obstacle(int x, int y);
void update_obstacle(Obstacle* obstacle);
void draw_obstacle(Obstacle* obstacle, char* buffer);
void delete_obstacle(Obstacle* obstacle);

//Screen limits for player (can't touch the stars on the sides - you don't want to go infinity! (props Rui Ribeiro) - each side stars zone has 50px of width
#define LEFT_LIMIT 50
#define RIGHT_LIMIT 750
#define PLAYER_START_Y 530

typedef struct{
	int alive; //0 if dead (which means game over), set (to 1?) if alive
	/*TO DO: add attribute to keep track of player's score (one for minutes and one for seconds,
	assuming score will be the amount of time he survives - easier to process, avoids unecessary calculations and extra time wasting*/
	/*TO DO: add information about bullet count (an array of MAX_BULLETS size?)*/
	Bitmap* bitmap;
}Player;

Player* create_player();
void update_player(Player* player, Mouse* mouse);
void draw_player(Player* player, char* buffer);
void delete_player(Player* player);

typedef enum gamestate_t {GAME_RUNNING, GAME_OVER} gamestate;

typedef struct{
	Obstacle** obstacles;
	Mouse* mouse;
	Keyboard* keyboard;
	Bitmap* background;
	Player* player;
	char* secondary_buffer;
	gamestate state;
}Game;

Game* create_game();
void update_game(Game* game);
void draw_game(Game* game);
void delete_game(Game* game);
