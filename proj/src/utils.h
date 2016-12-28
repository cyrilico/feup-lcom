#pragma once

/*! \file utils.h
 \brief General functions to aid the program flow
*/

/** @defgroup utils utils
 * @brief General functions to aid the program execution
 * @{
 *
 */

#define BIT(n) (0x01<<(n))
#define TWOSCOMPLEMENT(N) (short)(0xff00 | N)
#define ABS_VALUE(X) (X < 0 ? -X : X)
#define BCD_LSD(X) (X & 0x0000000F)
#define GAME_NUMBER_WIDTH 16
#define SCORE_BITMAP_WIDTH 30
#define SCORE_BITMAP_HEIGHT 30

/**
 * @brief Transforms an image filename into its absolute path
 *
 * @param filename Name of image file, with extension (ex: test.bmp)
 * @return absolute path of parameter
 */
const char* fullPath(const char* filename);

/**
 * @brief Draws a 2 digit number (always) in given coordinates, at given buffer
 *
 * Differs from draw_score_number as the bitmaps for numbers used here are different (16x24) and are used
 * during the main game
 *
 * @param number number to be drawn
 * @param x X-coordinate of top left corner position where number will be drawn
 * @param y Y-coordinate of top left corner position where number will be drawn
 * @param buffer Buffer where number will be drawn (normally, a secondary buffer)
 * @sa draw_score_number
 */
void draw_game_number(int number, int x, int y, char* buffer);

/**
 * @brief Transforms a number into its respective english word and adds .bmp extension to it
 *
 * This function is used to generate the filenames to be used in draw_score_number (filenames like 1.bmp were already taken
 * for main game numbers' bitmaps)
 *
 * @param number Number to be converted
 * @return number as an english word with .bmp extension (ex: 1 produces "one.bmp")
 * @sa draw_score_number
 */
char* number_to_string(int number);

/**
 * @brief Draws a 2 digit number (always) in given coordinates, at given buffer
 *
 * Differs from draw_game_number as the bitmaps for numbers used here are different (30x30) and are used
 * during the final score screen
 *
 * @param number Number to be drawn
 * @param x X-coordinate of top left corner position where number will be drawn
 * @param y Y-coordinate of top left corner position where number will be drawn
 * @param buffer Buffer where number will be drawn (normally, a secondary buffer)
 * @sa draw_game_number
 */
void draw_score_number(int number, int x, int y, char* buffer);

/**
 * @brief Draws the character ':' on given coordinates, at given buffer (used to draw time)
 *
 * @param x X-coordinate of top left corner position where colon will be drawn
 * @param y Y-coordinate of top left corner position where colon will be drawn
 * @param buffer Buffer where colon will be drawn (normally, a secondary buffer)
 * @sa draw_score_number
 */
void draw_colon(int x, int y, char* buffer);

/**
 * @brief Draws the character '/' on given coordinates, at given buffer (used to draw date)
 *
 * @param x X-coordinate of top left corner position where slash will be drawn
 * @param y Y-coordinate of top left corner position where slash will be drawn
 * @param buffer Buffer where slash will be drawn (normally, a secondary buffer)
 * @sa draw_score_number
 */
void draw_slash(int x, int y, char* buffer);

/**
 * @brief Draws a letter of the english alphabet on given coordinates, at given buffer
 *
 * @param x X-coordinate of top left corner position where letter will be drawn
 * @param y Y-coordinate of top left corner position where letter will be drawn
 * @param buffer Buffer where letter will be drawn (normally, a secondary buffer)
 */
void draw_letter(char letter, int x, int y, char* buffer);

/**
 * @brief Converts a number in BCD to its binary representation
 *
 * @param number_in_bcd Number to be converted
 * @return number converted to its binary representation
 */
unsigned long bcd_to_binary(unsigned long number_in_bcd);

/**
 * @brief Converts an array of numbers in BCD to their binary representation
 *
 * @param base Pointer to first element of array to be converted
 * @param size Number of elements in array to be converted
 * @return pointer to same array, now converted
 */
unsigned long* bcd_to_binary_array(unsigned long* base, int size);

/**
 * @brief Initializes graphic mode in mode 0x114 (800x600, 2 bytes per pixel)
 *
 * @returns 0 if successful initialization, -1 otherwise
 */
int start_graphic_mode();

/**
 * @brief Returns to MINIX's default text mode
 *
 * @returns 0 if successful exit, -1 otherwise
 */
int exit_graphic_mode();

/** @} end of utils */
