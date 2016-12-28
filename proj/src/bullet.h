#pragma once

/*! \file bullet.h
 \brief Functions related with processing bullets (lasers) shot by player in-game
*/

#include "player.h"
#include "bitmap.h"
#include "obstacle.h"

/** @defgroup bullet bullet
 * @brief Functions related with processing bullets (lasers) shot by player in-game
 * @{
 *
 *
 */

#define N_BULLETS 50
#define MAX_BULLETS_ON_SCREEN 10
#define BULLET_HEIGHT 12
#define BULLET_OFFSET 37 //Offset to Buzz's bitmap's top left corner
#define BULLET_SPEED 3
#define BULLET_GAIN_FACTOR 1

/**
 * \struct Bullet
 * @brief The Bullet "class" (Not really necessary, but good for code reading purposes)
 *
 */
typedef struct{
	Bitmap* bitmap; /*!< Holds image of bullet (laser) and its position */
}Bullet;

/**
 * @brief Creates a new bullet at given position
 *
 * The type of bullet drawn will depend on the player's bonus' information
 *
 * @param player Player 'object' to test bonus status
 * @param x X-coordinate where bullet will be created
 * @param y Y-coordinate where bullet will be created
 * @return Pointer to initialized Bullet
 */
Bullet* create_bullet(Player* player, int x, int y);

/**
 * @brief Checks if there's a collision between a certain bullet and a certain obstacle
 *
 * The function tests the top left and right pixels for the x-axis and the bullet's top for the y-axis. If its y-axis position is
 * aligned with the obstacle's and at least one of the x-axis positions if aligned aswell, it means there's a collision
 *
 * @param bullet Bullet 'object' to be tested
 * @param obstacle Obstacle 'object' to be tested
 * @return 1 if there's a collision, 0 otherwise
 */
int bullet_obstacle_collision(Bullet* bullet, Obstacle* obstacle);

/**
 * @brief Updates a bullet's position, checking if it goes off-screen in the process
 *
 * @param bullet Bullet 'object' to be updated and tested
 * @return 1 if bullet's new position is off-screen, 0 otherwise
 */
int update_bullet(Bullet* bullet);

/**
 * @brief Draws a bullet at given buffer
 *
 * @param bullet Bullet 'object' to be drawn
 * @param buffer Buffer where the bullet will be drawn at (normally a secondary buffer)
 */
void draw_bullet(Bullet* bullet, char* buffer);

/**
 * @brief Frees dynamically reserved memory and deletes bullet 'object'
 *
 * @param bullet Bullet 'object' to be deleted
 */
void delete_bullet(Bullet* bullet);

/** @} */
