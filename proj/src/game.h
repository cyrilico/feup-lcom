#pragma once

#include "player.h"
#include "obstacle.h"
#include "bitmap.h"
#include "mouse.h"
#include "keyboard.h"
#include "timer.h"
#include "video_gr.h"
#include "utils.h"

#define N_BULLETS 50
#define MAX_BULLETS_ON_SCREEN 5
#define BULLET_HEIGHT 12
#define BULLET_OFFSET 37
#define BULLET_SPEED 3
#define BULLET_GAIN_FACTOR 1


//Not really necessary, but good for reading purposes
typedef struct{
	Bitmap* bitmap;
}Bullet;

Bullet* create_bullet(int x, int y); //creates new bullet at position (x,y)
int bullet_obstacle_collision(Bullet* bullet, Obstacle* obstacle); //checks if bullet collided with object. returns 1 if yes, 0 otherwise
int update_bullet(Bullet* bullet); //updates bullet's position. returns 1 if bullet goes off-screen, 0 otherwise
void draw_bullet(Bullet* bullet, char* buffer);
void delete_bullet(Bullet* bullet);

typedef enum gamestate_t {GAME_RUNNING, GAME_SCORE, GAME_OVER} gamestate;
typedef enum gamedrawstate_t {DONTDRAW, DRAW} gamedrawstate;

typedef struct{
	Obstacle** obstacles;
	Mouse* mouse;
	Keyboard* keyboard;
	Bitmap* background;
	Bitmap* background_score;
	Player* player;
	char* secondary_buffer;
	Bullet** bullets;
	gamedrawstate drawstate;
	gamestate state;
}Game;

Game* create_game();
int determine_index(int bullet_x); //Determines the index of the obstacle (in the line) to test collision with (avoids testing for all and thus more calculations). Returns -1 if no obstacle in bullet's way
void update_game(Game* game);
void game_state_handler(Game* game);
void update_draw_state(Game* game);
void draw_game(Game* game);
int add_bullet_shot(Game* game, int x, int y); //Tries to shoot a bullet from player (whose position is (x,y)) by adding it to the game object (it may not be possible if MAX_BULLETS bullets are already on screen). Returns 1 if successful, 0 otherwise
void delete_game(Game* game);
