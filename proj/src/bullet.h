#pragma once

#include "player.h"
#include "bitmap.h"
#include "obstacle.h"

#define N_BULLETS 50
#define MAX_BULLETS_ON_SCREEN 10
#define BULLET_HEIGHT 12
#define BULLET_OFFSET 37
#define BULLET_SPEED 3
#define BULLET_GAIN_FACTOR 1
#define N_OBSTACLE_LINES 2
#define RANDOM_OBSTACLE_FACTOR 50

//Not really necessary, but good for reading purposes
typedef struct{
	Bitmap* bitmap;
}Bullet;

Bullet* create_bullet(Player* player, int x, int y); //creates new bullet at position (x,y)
int bullet_obstacle_collision(Bullet* bullet, Obstacle* obstacle); //checks if bullet collided with object. returns 1 if yes, 0 otherwise
int update_bullet(Bullet* bullet); //updates bullet's position. returns 1 if bullet goes off-screen, 0 otherwise
void draw_bullet(Bullet* bullet, char* buffer);
void delete_bullet(Bullet* bullet);
