#pragma once

/*! \file keyboard.h
 \brief Functions related with processing the i8042's keyboard
*/

#include "i8042.h"

/** @defgroup keyboard keyboard
 * @brief Functions related with processing the i8042's keyboard
 * @{
 *
 *
 */

/**
 * @brief Subscribes the i8042's keyboard and enables its interrupts
 *
 * @return Bitmask whose set bit number corresponds to the keyboard's hookID if successful, -1 otherwise
 */
int kbd_subscribe_int();

/**
 * @brief Unsubscribes the i8042's keyboard's interrupts
 *
 * @return 0 if successful, -1 otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief Transforms breakcodes into the respective letter
 *
 * @param code Breakcode received to be transformed
 * @return char corresponding to the breakcode received in case of letter breakcode, NULL character (0) otherwise
 */
char scancode_to_letter(unsigned long code);

/**
 * @brief Possible scancode receiving states (there are scancodes with more than one byte)
 *
 */
typedef enum scancodestate_t {READ_AGAIN, FULLY_READ} scancodestate;

/**
 * \struct Keyboard
 * @brief The Keyboard "class"
 *
 */
typedef struct{
	unsigned long scancode; /*!< Holds the current scancode */
	unsigned long scancode_aux; /*!< Holds the first scancode byte in case of two byte scancodes */
	scancodestate codestatus; /*!< Holds information about receiving the scancodes. Also serves as control */
}Keyboard;

/**
 * @brief Creates a new keyboard 'object'
 *
 * @return Pointer to initialized Keyboard
 */
Keyboard* create_keyboard();

/**
 * @brief Reads a scancode (or part of it) from OUT_BUF. Sets FULLY_READ flag if full scancode is read
 *
 * @param keyboard Keyboard 'object' where the scancode will be read to
 */
void read_scancode(Keyboard* keyboard);

/**
 * @brief Checks if a full scancode has been read (through attribute codestatus)
 *
 * @param keyboard Keyboard 'object' to be tested
 * @return 1 if keyboard's codestatus == RECEIVED, 0 otherwise
 */
int full_scancode_received(Keyboard* keyboard);

/**
 * @brief Checks if a given key has been pressed (or released, in case it's a break code)
 *
 * @param keyboard Keyboard 'object' to be tested
 * @param key Key to be tested
 * @return 1 if key was indeed detected (is stored in keyboard and its codestatus is RECEIVED), 0 otherwise
 */
int key_detected(Keyboard* keyboard, unsigned long key);

/**
 * @brief Frees dynamically reserved memory and deletes keyboard 'object'
 *
 * @param keyboard Keyboard 'object' to be deleted
 */
void delete_keyboard(Keyboard* keyboard);

/** @} */
