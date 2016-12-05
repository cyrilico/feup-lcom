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

	if(vg_init(0x114) == NULL)
		return -1;

	char* db_menu = (char*)(malloc(vg_get_window_size()));
	char* db_game = (char*)(malloc(vg_get_window_size()));

	Dispatcher* dispatcher = create_dispatcher(db_menu, db_game);

	//drawBitmap(background,vg_get_video_mem(),ALIGN_LEFT);
	//drawBitmap(buzz,vg_get_video_mem(),ALIGN_LEFT);
	//draw_mouse(mouse,vg_get_video_mem());


	while(dispatcher->state != EXIT_PROGRAM) {
		if(dispatcher->state == MAIN_MENU) {
			printf("Update Dispatcher\n");
			update_dispatcher_menu(dispatcher);
		}
		//else state == GAME
			//update_dispacher_game(dispatcher);
	}


	vg_exit();
	if(mouse_write_byte(DISABLE_MOUSE_DATA_REPORTING) == -1){
		printf("fodeu\n");
		return -1;
	}
	return mouse_unsubscribe_int();
}

