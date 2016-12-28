#pragma once

/*! \file rtc.h
 \brief Functions related with the usage of the PC's Real Time Clock
*/

#define CURRENT_TIME 0
#define CURRENT_DATE 1

/** @defgroup rtc rtc
 * @brief Functions related with the usage of the PC's Real Time Clock
 * @{
 *
 *
 */

/**
 * @brief Externally declared array containing a date
 *
 */
extern unsigned long* result_date;

/**
 * @brief Externally declared array containing a time
 *
 */
extern unsigned long* result_time;

/**
 * @brief Externally defined function that returns an array containing the current date or time
 *
 * @return Pointer to array containing either current date or current time
 */
extern unsigned long* rtc_get();

/**
 * @brief Subscribes Real Time Clock
 *
 * @return 0 if successful, -1 otherwise
 *
 */
int rtc_subscribe_int();

/**
 * @brief Unsubscribes Real Time Clock
 *
 * @return 0 if successful, -1 otherwise
 */
int rtc_unsubscribe_int();

/** @} */
