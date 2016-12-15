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
int update_obstacle(Obstacle* obstacle); //Updates obstacle position, checking if it goes off-screen aswell. Returns 0 if new position is inscreen, 1 otherwise
void draw_obstacle(Obstacle* obstacle, char* buffer);
int obstacle_off_screen(Obstacle* obstacle); //Checks if obstacle is out of the screen. Returns 1 if it is, 0 otherwise
void delete_obstacle(Obstacle* obstacle);

//Screen limits for player (can't touch the stars on the sides - you don't want to go infinity! (props Rui Ribeiro) - each side stars zone has 50px of width
#define LEFT_LIMIT 50
#define RIGHT_LIMIT 750
#define PLAYER_START_Y 460
#define PLAYER_DEATH_TOLERANCE 5

typedef struct{
	/*TO DO: add attribute to keep track of player's score (one for minutes and one for seconds,
	assuming score will be the amount of time he survives - easier to process, avoids unecessary calculations and extra time wasting)*/
	/*TO DO: add information about bullet count (an array of MAX_BULLETS size?)*/
	Bitmap** bullets;
	Bitmap* bitmap;
}Player;

Player* create_player();
void update_player_mouse(Player* player, Mouse* mouse, char* buffer); //Update player position based on latest mouse packet
void update_player_collision(Player* player, char* buffer); //Update player position in case of eventual collision
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
	Bitmap** bullets;
	gamestate state;
}Game;

Game* create_game();
void update_game(Game* game);
void draw_game(Game* game);
void remove_bullet_shot(Game* game); //Removes a bullet from Player's array and puts it into Game's array (only if there's at least one not NULL pointer in Player's)
void delete_game(Game* game);
