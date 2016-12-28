#pragma once

/*! \file timer.h
 \brief Functions for using the i8254 timer0
*/

/** @defgroup timer timer
 * @brief Functions for using the i8254 timer0
 * @{
 *
 */

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int timer_subscribe_int(void );

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int();

/** @} */
