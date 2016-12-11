/*#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
*/
#include "dispatcher.h"

  /* Current VESA graphics mode chosen: 0x114 (800x600, 16bit colors in RGB 5:6:5) */

int main(int argc, char **argv) {
	/* Initialize service */
	sef_startup();
	/*
	 * //Remove from commentary style if assembly files are used
	 * // Enable IO-sensitive operations for ourselves
	 * sys_enable_iop(SELF);
	*/

	srand(time(NULL));

	if(start_graphic_mode() != OK)
		return -1;

	Dispatcher* dispatcher = create_dispatcher();

	while(dispatcher->state != EXIT_PROGRAM) {
		if(dispatcher->state == MAIN_MENU) {
			printf("Update Dispatcher\n");
			process_main_menu(dispatcher);
		}
		else
			process_game(dispatcher);
	}

	delete_dispatcher(dispatcher);
	return exit_graphic_mode();
}

