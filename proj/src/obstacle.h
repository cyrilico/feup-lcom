#pragma once

#include "bitmap.h"

#define OBSTACLE_SPEED 3
#define OBSTACLE_WIDTH 71 //In reality it's 66 but this makes it so obstacles have a 5px gap between them (looks better and covers all game space)
#define OBSTACLE_HEIGHT 66
#define N_OBSTACLES 9
#define EMPTY_FACTOR 2

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
void generate_obstacle_line(Obstacle** obstacles, int line_size, int line_number); //generates a random obstacle line in array 'obstacles'
void delete_obstacle_line(Obstacle** obstacles, int line_size); //deletes entire obstacle line stored in 'obstacles'
