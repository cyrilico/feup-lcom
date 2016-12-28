#pragma once

/*! \file mouse
 \brief Functions related with processing the PC's mouse
*/

#include "bitmap.h"

/** @defgroup dispatcher dispatcher
 * Functions related with processing the PC's mouse
 * @{
 *
 *
 */

/**
 * @brief Possible mouse's left button states
 *
 */
typedef enum lbutton_t {RELEASED,PRESSED} lbstate;

/**
 * @brief Possible mouse's packet handling state
 *
 */
typedef enum packetstate_t {RECEIVING, RECEIVED} packetstate;


/**
 * \struct Mouse
 * @brief The Mouse "class"
 *
 */
typedef struct{
	lbstate left_button_state; /*!< Holds information about left button's pressing */
	int left_button_was_released; /*!< Holds information about left button being pressed. Non-zero when lbstate if RELEASED and the previous one was PRESSED */
	Bitmap* crosshair; /*!< Mouse's crosshair that will be drawn in menu screen */
	long packet_byte; /*!< Packets' bytes read from OUT_BUF */
	unsigned int byteID; /*!< Holds position to store a packet's byte in the array */
	unsigned char packet[3]; /*!< Current packet */
	packetstate packet_state; /*!< Holds information about packet receiving. Also serves as control for update functions */
}Mouse;

/**
 * @brief Subscribes to the PC's mouse and enables its interrupts
 *
 * @return Bitmask whose set bit number corresponds to the mouse's hookID if successful, -1 otherwise
 */
int mouse_subscribe_int();

/**
 * @brief Unsubscribes mouse's interrupts
 *
 * @return 0 if successful, -1 otherwise
 */
int mouse_unsubscribe_int();

/**
 * @brief Writes a command byte to a specified port
 *
 * @param destination Port where the command byte will be written to
 * @param cmd Command byte that will be written
 * @return 0 if command was sent successfully, -1 otherwise
 */
unsigned long mouse_write_code(long destination, unsigned char cmd);

/**
 * @brief Writes a command byte to IN_BUF issue a mouse command
 *
 * @param byte Command byte that will be written
 * @return 0 if command was sent successfully, -1 otherwise
 * @sa mouse_write_code
 */
int mouse_write_byte(unsigned char byte);

/**
 * @brief Creates a new mouse 'object' and enables the mouse's data reporting feature (through command byte ENABLE_MOUSE_DATA_REPORTING)
 *
 * @return Pointer to initialized Mouse
 * @sa mouse_write_byte
 */
Mouse* create_mouse();

/**
 * @brief Creates a new mouse 'object' but does not load the crosshair as it's unecessary in the main game and enables the mouse's data reporting feature (through command byte ENABLE_MOUSE_DATA_REPORTING)
 *
 * @return Pointer to initialized Mouse
 * @sa mouse_write_byte
 */
Mouse* create_game_mouse();

/**
 * @brief Draws a mouse 'object' at given buffer
 *
 * @param mouse Mouse 'object' to be drawn
 * @param buffer Buffer where the mouse will be drawn at (normally a secondary buffer)
 */
void draw_mouse(Mouse* mouse, char* buffer);

/**
 * @brief Reads a packet's byte from OUT_BUF and if it is the packet's last byte, updates the mouse
 *
 * @param mouse Mouse 'object' where the packet byte will be stored and possibly cause an update
 * @sa update_mouse
 */
void read_packet_byte(Mouse* mouse);

/**
 * @brief Updates a mouse's position and its left button pressing state and resets its packet receiving state information
 *
 * @param mouse Mouse 'object' to be updated
 */
void update_mouse(Mouse* mouse);

/**
 * @brief Checks if a mouse 'object' has received a full packet
 *
 * @param mouse Mouse 'object' to be tested
 * @return 1 if mouse's packet state is RECEIVED, 0 otherwise
 */
int full_packet_received(Mouse* mouse);

/**
 * @brief Resets a mouse's packet state to RECEIVING
 *
 * @param mouse Mouse 'object' to be updated
 */
void reset_packet_state(Mouse* mouse);

/**
 * @brief Frees dynamically reserved memory and deletes mouse 'object'
 *
 * @param mouse Mouse 'object' to be deleted
 */
void delete_mouse(Mouse* mouse);

/**
 * @brief Frees dynamically reserved memory (except for the un-initialized crosshair bitmap) and deletes mouse 'object'
 *
 * @param mouse Mouse 'object' to be deleted
 */
void delete_game_mouse(Mouse* mouse);

/** @} */
