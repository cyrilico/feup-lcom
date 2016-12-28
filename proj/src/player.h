#pragma once

/*! \file player.h
 \brief Functions related with processing the game's character
*/

#include "mouse.h"
#include "bitmap.h"

/** @defgroup player player
 * @brief Functions related with processing the game's character
 * @{
 *
 *
 */

#define BLACK 0
#define LEFT_LIMIT 75
#define RIGHT_LIMIT 725

#define N_BULLETS 50
#define PLAYER_START_X 75
#define PLAYER_START_Y 460

#define PLAYER_DEATH_TOLERANCE 5

#define NUMBER_OF_BONUSES 4
#define BONUS_DURATION 5
#define BONUS_FREQUENCY 10

/**
 * @brief Possible bonuses (including no bonus)
 *
 */
typedef enum bonusstate_t {NO_BONUS,INVINCIBLE,INFINITE_AMMO, DOUBLE_BULLETS} bonusstate;

/**
 * \struct Player
 * @brief The Player "class"
 *
 */
typedef struct{
	unsigned int score_minutes; /*!< Current player's minutes part of the score */
	unsigned int score_seconds; /*!< Current player's seconds part of the score */
	int score_aux; /*!< Counts timer0 interrupts to increment score at each second (really just counting time) */
	unsigned int number_of_bullets; /*!< Current number of bullets (lasers) player has left */
	unsigned int bonus_timer; /*!< Holds and controls bonus duration (initialized with BONUS_DURATION). Decremented until 0 every time score_seconds is incremented */
	bonusstate bonus; /*!< Current bonus information */
	Bitmap* bitmap; /*!< Player's normal appearance */
	Bitmap* bitmap_shield; /*!< Player's appearance when invincibility bonus is on (has a rectangle shield around him) */
}Player;

/**
 * @brief Creates a new playable character when a new game session is initialized
 *
 * @return Pointer to initialized Player 'object'
 */
Player* create_player();

/**
 * @brief Updates a player's position based on latest mouse packet received
 *
 * @param player Player 'object' to be updated
 * @param mouse Mouse 'object' used in game which provides information for the update
 * @param buffer Buffer where player will be drawn at its new coordinates (normally a secondary buffer)
 */
void update_player_mouse(Player* player, Mouse* mouse, char* buffer);

/**
 * @brief Updates a player's position based on a possible collision with an obstacle
 *
 * Instead of calculating the possible obstacle to test collision with (would require too many calculations as player's position changes
 * too often), the colors of pixels on top the player's bitmap are tested. If non-black, an obstacle must be there pushing the player down with it
 * This function ignores possible obstacles in the way if player's current bonus is INVINCIBLE
 *
 * @param player Player 'object' to be updated
 * @param buffer Buffer where player will be drawn at its new coordinates (normally a secondary buffer)
 */
void update_player_collision(Player* player, char* buffer);

/**
 * @brief Decrements a player's number of bullets, unless he has an INFINITE_AMMO bonus activated
 *
 * @param player Player 'object' to be updated
 */
void update_number_of_bullets(Player* player);

/**
 * @brief Checks if given player still has bullets (lasers) left
 *
 * @param player Player 'object' to be updated
 * @return 1 if player's number of bullets is > 0 or current bonus is INFINITE_AMMO, 0 otherwise
 */
int player_has_bullets(Player* player);

/**
 * @brief Randomly generates a bonus and activates it immediately
 *
 * @param player Player 'object' to be updated
 */
void generate_bonus(Player* player);

/**
 * @brief Updates a player's score
 *
 * @param player Player 'object' to be updated
 */
void update_player_score(Player* player);

/**
 * @brief Updates a player's bonus duration (decrements it). If duration ends (reaches 0), bonus is removed
 *
 * @param player Player 'object' to be updated
 */
void update_player_bonus(Player* player);

/**
 * @brief Draws a player at given buffer
 *
 * @param player Player 'object' to be drawn
 * @param buffer Buffer where player will be drawn (normally a secondary buffer)
 */
void draw_player(Player* player, char* buffer);

/**
 * @brief Frees dynamically reserved memory for a player and deletes the 'object'
 *
 * @param player Player 'object' to be deleted
 */
void delete_player(Player* player);

/** @} */
