#pragma once

#include "mouse.h"
#include "bitmap.h"

#define N_BULLETS 50

//Screen limits for player (can't touch the stars on the sides - you don't want to go infinity! (props Rui Ribeiro) - each side stars zone has 75px of width
#define LEFT_LIMIT 75
#define RIGHT_LIMIT 725
#define PLAYER_START_Y 460
#define PLAYER_DEATH_TOLERANCE 5
#define NUMBER_OF_BONUSES 3 /* TO DO: Change this to a more random function (don't just use %) */
#define BONUS_DURATION 5
#define BONUS_FREQUENCY 10 //every 10 seconds, a new bonus

typedef enum bonusstate_t {NO_BONUS,INVINCIBLE,INFINITE_AMMO} bonusstate;

typedef struct{
	unsigned int score_minutes;
	unsigned int score_seconds;
	int score_aux; //Counts timer0 interrupts to increment score at each second (really just counting the time...)
	unsigned int number_of_bullets;
	unsigned int bonus_timer; //holds and controls bonus duration (set to macro BONUS_DURATION, which holds the default value in seconds). is decremented until 0 every time score_seconds is incremented
	bonusstate bonus;
	Bitmap* bitmap;
	Bitmap* bitmap_shield;
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
