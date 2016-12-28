#pragma once

/*! \file menu.h
 \brief Functions related with processing the program's main menu
*/

#include "bitmap.h"
#include "mouse.h"
#include "keyboard.h"

/** @defgroup dispatcher dispatcher
 * Functions related with processing the program's main menu
 * @{
 *
 *
 */

#define PLAY_BUTTON_X 300
#define PLAY_BUTTON_Y 280
#define EXIT_BUTTON_X 300
#define EXIT_BUTTON_Y 345
#define BUTTON_WIDTH 145
#define BUTTON_HEIGHT 50

/**
 * \struct Button
 * @brief The Button "class" (used for buttons present in main menu)
 *
 */
typedef struct{
	int xi; /*!< Button's top left corner x-coordinate */
	int yi; /*!< Button's top left corner y-coordinate */
	int width; /*!< Button's width */
	int height; /*!< Button's height */
}Button;

/**
 * @brief Creates a new button of given dimensions at given position
 *
 * @param x X-coordinate of top left corner of new button
 * @param y Y-coordinate of top left corner of new button
 * @param width New button's width
 * @param height New button's height
 * @return Pointer to initialized Button
 */
Button* create_button(int x, int y, int width, int height);

/**
 * @brief Checks if given mouse is hovering over given button
 *
 * @param button Button 'object' to be tested
 * @param mouse Mouse 'object' to be tested
 * @return 1 if mouse is hovering over button, 0 otherwise
 */
int mouse_on_button(Button* button, Mouse* mouse);

/**
 * @brief Frees dynamically reserved memory and deletes 'object'
 *
 * @param button Button 'object' to be deleted
 */
void delete_button(Button* button);

#define KBD_UPDATE 0
#define MOUSE_UPDATE 1

/**
 * @brief Possible Menu choosing options' states
 *
 */
typedef enum menustate_t {NOT_DONE, PLAY_CHOSEN, EXIT_CHOSEN} menustate;

/**
 * \struct Menu
 * @brief The Menu "class"
 *
 */
typedef struct{
	Button* play_button; /*!< Holds information about PLAY button */
	Button* exit_button; /*!< Holds information about EXIT button */
	Mouse* mouse; /*!< Mouse to be shown and used to select and option */
	Keyboard* keyboard; /*!< Keyboard to store scancodes read (all are ignored except ESC_BREAK) */
	Bitmap* background; /*!< The menu background */
	char* secondary_buffer; /*!< The double buffer used for the menu */
	menustate state; /*!< The choosing options' state information */
}Menu;

/**
 * @brief Creates a new main menu
 *
 * @return Pointer to initialized Menu
 */
Menu* create_menu();

/**
 * @brief Copies frame stored in menu's secondary buffer to the main buffer (video_mem)
 *
 * @param menu Menu 'object' holding next frame to be drawn
 */
void draw_menu(Menu* menu);

/**
 * @brief Updates a menu according to latest information received either from mouse or keyboard
 *
 * Updates the attribute 'state' (transitions to game state if PLAY has been chosen, exits program if ESC_BREAK detected
 * or EXIT has been chosen)
 *
 * @param menu Menu 'object' to be updated
 * @param kbd_or_mouse 0 if update is due to keyboard information, 1 if caused by mouse information
 */
void update_menu(Menu* menu, int kbd_or_mouse);

/**
 * @brief Frees dynamically reserved memory and deletes 'object'
 *
 * @param menu Menu 'object' to be deleted
 */
void delete_menu(Menu* menu);

/** @} */
