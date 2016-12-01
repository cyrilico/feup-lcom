#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "video.h"
#include "sprite.h"
#include "i8042.h"
#include "bitmap.h"
#include "keyboard.h"
#include "game.h"

  /*
     * -------------------
     * VESA graphics modes
     *      16-bit (5:6:5)
     * -------------------
     *   320×200 - 0x10E
     *   640×480 - 0x111
     *   800×600 - 0x114
     *  1024×768 - 0x117
     * 1280×1024 - 0x11A
     * -------------------
     */

int main(int argc, char **argv) {
	sef_startup();

	if(vg_init(0x114) == NULL)
		return -1;

	Bitmap* buzz = loadBitmap("/home/lcom/feup2/lcom1617-t4g11/proj/images/buzzie.bmp");
	buzz->x = 400;
	buzz->y = 400;
	drawBitmap(buzz, buzz->x, buzz->y, ALIGN_CENTER);

	 Game* game = (Game*) start_game();
	 while(!game->done) {
		 update_game(game);

		 if(!game->done) {
			 if(game->draw) {
				// draw_game(game);
			    	if(game->scancode == A_MAKE){
			    		vg_fill_screen(buzz->x,buzz->y,buzz->bitmapInfoHeader.width,buzz->bitmapInfoHeader.height,0);
			    		buzz->x -= 5;
			    		drawBitmap(buzz, buzz->x, buzz->y, ALIGN_LEFT);
			    	}
			    	else if(game->scancode == D_MAKE){
			    		vg_fill_screen(buzz->x,buzz->y,buzz->bitmapInfoHeader.width,buzz->bitmapInfoHeader.height,0);
			    		buzz->x += 5;
			    		drawBitmap(buzz, buzz->x, buzz->y, ALIGN_LEFT);
			    	}
			 //flipMBuffer();
			 //flipDisplay();
		 }
	 }
}
	 exit_game(game);
	//timer_test_int(10);
	vg_exit();
    return 0;
}

