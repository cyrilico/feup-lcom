#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "utils.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "video.h"
#include "sprite.h"
#include "i8042.h"
#include "bitmap.h"
#include "keyboard.h"

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

	Bitmap* buzz = loadBitmap(fullPath("buzzie.bmp"),400,400);
	while(buzz->x < 600){
		vg_fill_screen(0,0,vg_get_h_res(),vg_get_v_res(),0);
		drawBitmap(buzz, ALIGN_LEFT);
		buzz->x += 20;
		tickdelay(micros_to_ticks(20000*5*10));
	}


	vg_exit();
	printf("Acabei\n");
    return 0;
}

