#pragma once

/*! \file score.h
 \brief Functions related with game score handling
*/

#define MAX_SCORES_READ 15
#define NAME_LENGTH 4
#define DATE_LENGTH 3
#define TIME_LENGTH 3
#define MAX_SCORES_ON_SCREEN 5
#define SESSION_SCORE_X_START 332
#define SESSION_SCORE_Y 145
#define HIGHSCORE_GAP 30

/** @defgroup score score
 * @brief Functions related with game score handling
 * @{
 *
 *
 */


/**
 * \struct Score
 * @brief The Score "class"
 *
 */
typedef struct{
	unsigned int points_minutes; /*!< The minutes part of a score */
	unsigned int points_seconds; /*!< The seconds part of a score */
	unsigned long time[DATE_LENGTH]; /*!< The time when the score was obtained */
	unsigned long date[TIME_LENGTH]; /*!< The date when the score was obtained */
	char name[NAME_LENGTH]; /*!< The name of the session when the score was obtained */
} Score;

/**
 * @brief Creates a new score from a recently finished game
 *
 * @param new_points_minutes The new score's minutes value
 * @param new_points_seconds The new score's seconds value
 * @param new_time Pointer to array that contains time when the score was obtained
 * @param new_date Pointer to array that contains date when the score was obtained
 * @param new_name String that holds name of the session when the score was obtained
 * @return Pointer to newly created score
 */
Score* create_score(unsigned int new_points_minutes, unsigned int new_points_seconds, unsigned long* new_time, unsigned long* new_date, char* new_name);

/**
 * @brief Sets a score's session name
 *
 * @param new_name name of the session
 */
void set_score_name(Score* score, char* new_name);

/**
 * @brief Draws a score on given coordinates, at given buffer
 *
 * Function takes coordinates of top left corner of score's session name's first letter, as the positions of the remaining characters is
 * automatically calculated
 *
 * @param score Score to be drawn
 * @param x x-coordinate of top left corner of first letter of score's session name
 * @param y y-coordinate of top left corner of first letter of score's session name
 * @param buffer buffer where the score will be drawn (normally a secondary buffer)
 */
void draw_score(Score* score, int x, int y, char* buffer);

/**
 * @brief Draws an array of (high)scores on given coordinates, at given buffer
 *
 * Function takes coordinates of top left corner of first score's session name's first letter, as the positions of the remaining characters
 * (in the same score) and the next scores are automatically calculated
 *
 * @param scores Pointer to array of Score 'objects' to be drawn
 * @param x x-coordinate of top left corner of first letter of first score's session name
 * @param y y-coordinate of top left corner of first letter of first score's session name
 * @param buffer buffer where the score will be drawn (normally a secondary buffer)
 */
void draw_scores(Score** scores, int x, int y, char* buffer);

/**
 * @brief Provides absolute path of highscores text file (file must be in proj/src)
 *
 * @return absolute path of "highscores.txt"
 */
const char* get_scores_file();

/**
 * @brief Writes a score to the highscores text file (appended to existing ones)
 *
 * @param score Score to be saved in text file
 */
void write_score_to_file(Score* score);

/**
 * @brief Reads current scores saved in highscores text file
 *
 * @return Pointer to array of scores read
 */
Score** read_scores_from_file();

/**
 * @brief Compares two scores by their total score in seconds (used in qsort to sort highscores array)
 *
 * @param s1 First of two scores to be compared
 * @param s2 Second of two scores to be compared
 * @return -1 if s1's total score is lower than s2's total score, 1 if it's bigger, 0 if equal
 */
int comp_score(const void* s1, const void* s2);

/**
 * @brief Frees memory dynamically reserved for a score
 *
 * @param score Score 'object' to be deleted
 */
void delete_score(Score* score);

/** @} */
