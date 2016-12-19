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
#define N_BULLETS 50
#define MAX_BULLETS_ON_SCREEN 5
#define BULLET_HEIGHT 12
#define BULLET_OFFSET 37
#define OBSTACLE_SPEED 3
#define BULLET_SPEED 3
#define EMPTY_FACTOR 2
#define BULLET_GAIN_FACTOR 1

typedef struct{
	unsigned int const_lives; //Same as 'lives' but will never be modified
	unsigned int lives;
	Bitmap* bitmaps[3]; //Different bitmap according to number of lives
}Obstacle;

Obstacle* create_obstacle(int x, int y);
int update_obstacle(Obstacle* obstacle); //Updates obstacle position, checking if it goes off-screen aswell. Returns 0 if new position is inscreen, 1 otherwise
void draw_obstacle(Obstacle* obstacle, char* buffer);
int obstacle_off_screen(Obstacle* obstacle); //Checks if obstacle is out of the screen. Returns 1 if it is, 0 otherwise
void delete_obstacle(Obstacle* obstacle);
void generate_obstacle_line(Obstacle** obstacles, int line_size); //generates a random obstacle line in array 'obstacles'
void delete_obstacle_line(Obstacle** obstacles, int line_size); //deletes entire obstacle line stored in 'obstacles'

//Screen limits for player (can't touch the stars on the sides - you don't want to go infinity! (props Rui Ribeiro) - each side stars zone has 50px of width
#define LEFT_LIMIT 75
#define RIGHT_LIMIT 725
#define PLAYER_START_Y 460
#define PLAYER_DEATH_TOLERANCE 5
#define NUMBER_OF_BONUSES 4
#define BONUS_DURATION 5
#define BONUS_FREQUENCY 10 //every 10 seconds, a new bonus

typedef enum bonusstate_t {NO_BONUS,INVINCIBLE,INFINITE_AMMO} bonusstate; /* TO DO: Add bitmap when invincible (rectangle around him?) and draw it when it's on */

typedef struct{
	unsigned int score_minutes;
	unsigned int score_seconds;
	int score_aux; //Counts timer0 interrupts to increment score at each second (really just counting the time...)
	unsigned int number_of_bullets;
	unsigned int bonus_timer; //holds and controls bonus duration (set to macro BONUS_DURATION, which holds the default value in seconds). is decremented until 0 every time score_seconds is incremented
	bonusstate bonus;
	Bitmap* bitmap;
}Player;

Player* create_player();
void update_player_mouse(Player* player, Mouse* mouse, char* buffer); //Update player position based on latest mouse packet
void update_player_collision(Player* player, char* buffer); //Update player position in case of eventual collision (unless current bonus is INVINCIBLE)
void update_number_of_bullets(Player* player); //Decrements number of bullets of 'player' (unless current bonus is INFINITE_AMMO)
int player_has_bullets(Player* player); //Returns 1 if player's number of bullets is > 0 or if current bonus is INFINITE_AMMO, 0 otherwise
void generate_bonus(Player* player);
void update_player_score(Player* player);
void update_player_bonus(Player* player); //removes bonus if bonus_timer reaches 0
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
typedef enum gamedrawstate_t {DONTDRAW, DRAW} gamedrawstate;

typedef struct{
	Obstacle** obstacles;
	Mouse* mouse;
	Keyboard* keyboard;
	Bitmap* background;
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
