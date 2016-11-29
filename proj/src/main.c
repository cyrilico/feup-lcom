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

	Bitmap* buzz = loadBitmap("/home/lcom/feup2/lcom1617-t4g11/proj/images/buzz1.bmp");

	drawBitmap(buzz, 100, 100, ALIGN_CENTER);

	timer_test_int(10);
	vg_exit();
    return 0;
}
