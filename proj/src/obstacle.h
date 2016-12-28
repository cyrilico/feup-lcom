#pragma once

/*! \file obstacle.h
 \brief Functions related with processing the game's obstacles
*/

/** @defgroup dispatcher dispatcher
 * Functions related with processing the game's obstacles
 * @{
 *
 *
 */

#include "bitmap.h"

#define OBSTACLE_SPEED 3
#define OBSTACLE_WIDTH 71 //In reality it's 66 but this makes it so obstacles have a 5px gap between them (looks better and covers all game space)
#define OBSTACLE_HEIGHT 66
#define N_OBSTACLES 9
#define EMPTY_FACTOR 2
#define MAX_OBSTACLE_LIVES 5

/**
 * @brief Holds current maximum value of lives that newly generated obstacles have
 *
 */
static unsigned int current_max_lives;

/**
 * @brief Holds current minimum value of lives that newly generated obstacles have
 *
 */
static unsigned int current_min_lives;

/**
 * \struct Obstacle
 * @brief The Obstacle "class"
 *
 */
typedef struct{
	unsigned int const_lives; /*!< Same initial value as 'lives' but will never be modified (need value stored to increment player's number of bullets later) */
	unsigned int lives; /*!< Holds obstacle's current number of lives left */
	Bitmap* bitmaps[MAX_OBSTACLE_LIVES]; /*!< Holds possibilities of obstacle appearance. Different bitmap according to current number of lives */
}Obstacle;

/**
 * @brief Creates a new obstacle with a random number of lives at given position
 *
 * @param x X-coordinate of top left corner of new obstacle's initial position
 * @param y Y-coordinate of top left corner of new obstacle's initial position
 * @return Pointer to newly created obstacle
 */
Obstacle* create_obstacle(int x, int y);

/**
 * @brief Updates an obstacle's position, checking in the process if it goes off-screen
 *
 * @param obstacle Obstacle 'object' to be updated
 * @return 1 if obstacle's top left corner position is off-screen, 0 otherwise
 */
int update_obstacle(Obstacle* obstacle);

/**
 * @brief Draws an obstacle at given buffer
 *
 * @param obstacle Obstacle 'object' to be drawn
 * @param buffer Buffer where obstacle will be drawn (normally a secondary buffer)
 */
void draw_obstacle(Obstacle* obstacle, char* buffer);

/**
 * @brief Checks if a given obstacle's top left corner is off-screen
 *
 * @param obstacle Obstacle 'object' to be tested
 * @return 1 if obstacle's top left corner is off-screen, 0 otherwise
 */
int obstacle_off_screen(Obstacle* obstacle);

/**
 * @brief Frees memory dynamically reserved for an obstacle and deletes it
 *
 * @param obstacle Obstacle 'object' to be delete
 */
void delete_obstacle(Obstacle* obstacle);

/**
 * @brief Generates a line of obstacles of specified size and position on the screen
 *
 * @param obstacles Pointer to dynamically reserved but yet un-initialized obstacle array
 * @param line_size Amount of obstacles to be generated
 * @param line_number offset to first line on-screen. For each increment here, the distance of the line to the screen top grows of OBSTACLE_HEIGHT
 */
void generate_obstacle_line(Obstacle** obstacles, int line_size, int line_number);

/**
 * @brief Initializes private global variable current_max_lives to 1
 */
void init_current_max_lives();

/**
 * @brief Initializes private global variable current_min_lives to 1
 */
void init_current_min_lives();

/**
 * @brief Updates the private global variables current_min_lives and current_max_lives to increase average number of lives of newly created obstacles
 * @sa init_current_max_lives
 * @sa init_current_min_lives
 */
void update_lives_boundaries();

/**
 * @brief Frees dynamically reserved memory and deletes entire line of obstacles
 *
 * @param obstacles Pointer to Obstacle 'objects' array
 * @param line_size Amount of obstacle to be deleted
 */
void delete_obstacle_line(Obstacle** obstacles, int line_size);

/** @} */
