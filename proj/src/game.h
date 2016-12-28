#pragma once

/*! \file game.h
 \brief Functions related with processing the main game
*/

#include "mouse.h"
#include "keyboard.h"
#include "score.h"
#include "bullet.h"

/** @defgroup dispatcher dispatcher
 * Functions related with processing the main game
 * @{
 *
 *
 */

#define PLAYER_NAME_X_START 300
#define PLAYER_NAME_Y_START 266
#define PAUSE_MESSAGE_X 300
#define PAUSE_MESSAGE_Y 264
#define UNDERSCORE_GAP 67
#define HIGHSCORE_NAME_X 120
#define HIGHSCORE_NAME_Y 57

/**
 * @brief Possible game states
 *
 */
typedef enum gamestate_t {GAME_RUNNING, GAME_PAUSED, GAME_SCORE, GAME_OVER} gamestate;

/**
 * @brief Possible game session's name receiving states
 *
 */
typedef enum scorenamestate_t {FIRST_LETTER, SECOND_LETTER, THIRD_LETTER, FOURTH_LETTER, NAME_RECEIVED} scorenamestate;

/**
 * @brief Possible game drawing states
 *
 */
typedef enum gamedrawstate_t {DONTDRAW, DRAW} gamedrawstate;

/**
 * \struct Game
 * @brief The Game "class"
 *
 */
typedef struct{
	Obstacle*** obstacles; /*!< Matrix holding information about current on-screen obstacles */
	Mouse* mouse; /*!< Holds information about mouse movement to update the player's position */
	Keyboard* keyboard; /*!< Holds information about keys pressed that will afect the game's flow */
	Bitmap* background; /*!< The game background */
	Player* player; /*!< The main character */
	char* secondary_buffer; /*!< The double buffer used in the game */
	Bullet** bullets; /*!< Array that holds information about current on-screen bullets (by other words, bullets shot by player) */
	gamedrawstate drawstate; /*!< Holds information about the game's drawing state */
	gamestate state; /*!< Holds information about the game's state */
	Score* session_score; /*!< Will hold information about the game session when it ends */
	Score** current_highscores; /*!< Array that holds information about previous game sessions */
	scorenamestate namestate; /*!< Holds information about current session's name letter receiving */
	char session_name[NAME_LENGTH+1]; /*!< String that will store current session's name (read from user input) */
}Game;

/**
 * @brief Creates a new game session
 *
 * @return Pointer to initialized Game
 */
Game* create_game();

/**
 * @brief Calculates the index of the obstacle to test collision with (used for bullet-obstacle collisions)
 *
 * This functions avoids an unecessary amount of calculations (for every bullet, check every obstacle until one is in its way)
 * which would slow down the game pace a lot
 *
 * @param bullet_x X-coordinate of bullet to test obstacle alignment with
 * @return index of obstacle in the array in case there's an obstacle in the bullet's way, -1 if clear path
 */
int determine_index(int bullet_x);

/**
 * @brief Updates a game session that hasn't finished yet
 *
 * More specifically, this function:
 * - Updates each obstacle's position in the obstacle matrix, generating a new matrix of obstacles if the old one goes off-screen and the game is still running
 * - Uses determine_index to check if a bullet has collided with an obstacle, and, if so, delete the bullet and update the obstacle. This is done for all bullets
 * - Updates the player's position in case of a collision with an object
 * - Updates the player's score (this function is called every timer0 interrupt)
 * - Generates a new bonus for the player, if it is the case
 * - Updates the game's difficulty based on average number of lives of newly created obstacles, if it is the case
 * - Prepares the next frame to be drawn
 * - Detects if game session has ended (if player gets pushed off-screen by an obstacle)
 *
 * @param game Game 'object' to be updated
 */
void update_game_running(Game* game);

/**
 * @brief Updates the current session's name letter receiving state machine according to latest key received from user input
 *
 * @param game Game 'object' to be updated
 * @param current_key Latest key obtained from user input
 */
void game_score_event_handler(Game* game, char current_key);

/**
 * @brief Updates a game session that has finished (and therefore is on the final score screen)
 *
 * More specifically, this function:
 * - Calls game_score_event_handler (this function every time there is a new user input)
 * - Prepares the next frame to be drawn
 *
 * @param game Game 'object' to be updated
 * @sa game_score_event_handler
 */
void update_game_score(Game* game);

/**
 * @brief Checks if a game has ended (if player got pushed off-screen by an obstacle). Updates the attribute gamestate if necessary
 *
 * @param game Game 'object' to be updated
 * @sa gamestate
 */
void detect_game_end(Game* game);

/**
 * @brief Every time it is called it flips the game's attribute drawstate. Options are DONTDRAW and DRAW (guarantees 30fps game)
 *
 * @param game Game 'object' to be updated
 * @sa gamedrawstate
 */
void update_draw_state(Game* game);

/**
 * @brief Copies frame stored in menu's secondary buffer to the main buffer (video_mem)
 *
 * @param game Game 'object' where the next frame to be drawn is stored
 */
void draw_game(Game* game);

/**
 * @brief Tries to shoot a bullet from the player's position
 *
 * Note that it may not be possible to shoot a bullet if there are already MAX_BULLETS_ON_SCREEN bullets on-screen
 * This limit was implemented to avoid too many calculations that could cause lag
 *
 * @param game Game 'object' where the bullet will possibly be shot from
 * @param x X-coordinate of player's bitmap's top left corner
 * @param y Y-coordinate of player's bitmap's top left corner
 * @return 1 if successfully shot bullet, 0 otherwise
 */
int add_bullet_shot(Game* game, int x, int y);

/**
 * @brief Frees dynamically reserved memory and deletes 'object'
 *
 * @param game Game 'object' to be deleted
 */
void delete_game(Game* game);

/** @} */
