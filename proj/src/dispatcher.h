#pragma once

/*! \file dispatcher.h
 \brief Functions related with the program's state and HW interruptions handling
*/

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "menu.h"
#include "game.h"

/** @defgroup dispatcher dispatcher
 * @brief Functions related with the program's state and HW interruptions handling
 * @{
 *
 *
 */

/**
 * @brief Possible Dispatcher's states
 *
 */
typedef enum dispatcherstate_t {MAIN_MENU,GAME,EXIT_PROGRAM} dispatcherstate;

/**
 * \struct Dispatcher
 * @brief The Dispatcher "class"
 *
 */
typedef struct{
	int irq_timer; /*!< Timer0's irq after respective interrupt subscription */
	int irq_kbd; /*!< Keyboard's irq after respective interrupt subscription */
	int irq_mouse; /*!< Mouse's irq after respective interrupt subscription */
	int irq_rtc; /*!< RTC's irq after respective interrupt subscription */

	message msg;
	int ipc_status;
	char* pwd; /*!< String that holds the session's path - value inserted by user as service run call's argument */
	dispatcherstate state; /*! The dispatcher's current state */
} Dispatcher;

/**
 * @brief Subscribes to hardware peripherals and initializes Dispatcher's members. Also initializes variable used to store the session's path
 *
 * @param path String that holds the session's path - value inserted by user as service run call's argument
 * @return Pointer to initialized Dispatcher
 */
Dispatcher* create_dispatcher(char* path);

/**
 * @brief Frees dynamically allocated memory by Dispatcher's members
 * @param dispatcher Dispatcher to delete
 */
void delete_dispatcher(Dispatcher* dispatcher);

/**
 * @brief Returns the repository's parent directory path
 * @return String with the repository's parent directory path
 */
char* get_pwd();

/**
 * @brief Initializes either game or menu based on dispatcher's state.
 * Includes driver receive loop that calls game specific functions to handle hardware interruptions, which loops until the dispatcher's state changes
 * @param Dispatcher whose members will be changed
 * @sa state_handler
 */
void interrupt_handler(Dispatcher* dispatcher);

/**
 * @brief State handler to determine dispatcher's next state, based on menu or game's states. Also deletes either menu or game if the dispatcher's state changes.
 * @param dispatcher Dispatcher whose next state is being determined.
 * @param menu Menu whose state may determine the dispatcher's next state. May be deleted
 * @param game Game whose state may determine the dispatcher's next state. May be deleted
 */
void state_handler(Dispatcher* dispatcher, Menu* menu, Game* game);

/** @} */ // end of dispatcher
