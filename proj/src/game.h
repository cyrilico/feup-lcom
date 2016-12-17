#pragma once

#include "bitmap.h"
#include "mouse.h"
#include "keyboard.h"
#include "timer.h"
#include "video_gr.h"
#include "utils.h"

#define OBSTACLE_WIDTH 71 //In reality it's 66 but this makes it so obstacles have a 5px gap between them (looks better and covers all game space)
#define OBSTACLE_HEIGHT 66
#define N_OBSTACLES 9
#define BLACK 0
#define N_BULLETS 100
#define MAX_BULLETS_ON_SCREEN 5
#define BULLET_HEIGHT 12
#define BULLET_OFFSET 37
#define OBSTACLE_SPEED 3
#define BULLET_SPEED 3
#define EMPTY_FACTOR 2

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
#define LEFT_LIMIT 75
#define RIGHT_LIMIT 725
#define PLAYER_START_Y 460
#define PLAYER_DEATH_TOLERANCE 5

typedef struct{
	/*TO DO: add attribute to keep track of player's score (one for minutes and one for seconds,
	assuming score will be the amount of time he survives - easier to process, avoids unecessary calculations and extra time wasting)*/
	unsigned int number_of_bullets;
	Bitmap* bitmap;
}Player;

Player* create_player();
void update_player_mouse(Player* player, Mouse* mouse, char* buffer); //Update player position based on latest mouse packet
void update_player_collision(Player* player, char* buffer); //Update player position in case of eventual collision
void draw_player(Player* player, char* buffer);
void delete_player(Player* player);

//Not really necessary, but good for reading purposes
typedef struct{
	Bitmap* bitmap;
}Bullet;

Bullet* create_bullet(int x, int y); //creates new bullet at position (x,y)
int bullet_obstacle_collision(Bullet* bullet, Obstacle* obstacle); //checks if bullet collided with object. returns 1 if yes, 0 otherwise
int update_bullet(Bullet* bullet); //updates bullet's position. returns 1 if bullet goes off-screen, 0 otherwise
void draw_bullet(Bullet* bullet, char* buffer);
void delete_bullet(Bullet* bullet);

typedef enum gamestate_t {GAME_RUNNING, GAME_OVER} gamestate;

typedef struct{
	Obstacle** obstacles;
	Mouse* mouse;
	Keyboard* keyboard;
	Bitmap* background;
	Player* player;
	char* secondary_buffer;
	Bullet** bullets;
	gamestate state;
}Game;

Game* create_game();
void update_game(Game* game);
void draw_game(Game* game);
int add_bullet_shot(Game* game, int x, int y); //Tries to shoot a bullet at (x,y) by adding it to the game object (it may not be possible if MAX_BULLETS bullets are already on screen). Returns 1 if successful, 0 otherwise
void delete_game(Game* game);
