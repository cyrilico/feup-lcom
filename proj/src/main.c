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
	int kbd = kbd_subscribe_int();
	if(kbd == -1){
		vg_exit();
		printf("error in subscribing kbd\n");
		return -1;
	}
	int window_size = vg_get_h_res()*vg_get_v_res()*2;
	char* double_buffer = (char*)(malloc(window_size));
	Bitmap* background = loadBitmap(fullPath("menu_background.bmp"),0,0);
	Bitmap* buzz = loadBitmap(fullPath("buzzie.bmp"),0,vg_get_v_res()/2);
	buzz->x = vg_get_h_res()-buzz->bitmapInfoHeader.width;
	int ipc_status;
	message msg;
	unsigned long code = 0;
	unsigned long code_aux = 0;
	int read_again = 0;
	int ignore = 0;
	int r;
	unsigned long trash = 0;
	while(trash != 0x9C){
		trash = kbd_read_code();
		tickdelay(micros_to_ticks(DELAY_US));
	}
	while((buzz->x > 0) && (code != ESC_BREAK)){
		//vg_fill_screen(0,0,vg_get_h_res(),vg_get_v_res(),0);
		drawBitmap(background, double_buffer, ALIGN_LEFT);
		drawBitmap(buzz, double_buffer, ALIGN_LEFT);
		memcpy(vg_get_video_mem(),double_buffer,window_size);
		buzz->x -= 20;
	/*	if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
			ignore = 1;
	*/	if (is_ipc_notify(ipc_status) && !ignore) { // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: // hardware interrupt notification
				if (msg.NOTIFY_ARG & kbd) { // subscribed interrupt
					code = kbd_read_code();
					if(code == -1)
						return -1;
					else if(read_again == 1){
						code = code << 8;
						code |= code_aux;
						read_again = 0;
					}
					else if(code == TWO_BYTE_SCANCODE){
						code_aux = code;
						read_again = 1;
					}
				}
				break;
			default:
				break; // o other notifications expected: do nothing
			}
		}
		tickdelay(micros_to_ticks(5*DELAY_US));
	}
	kbd_unsubscribe_int();
	vg_exit();
	code = kbd_read_code();
	if(code == 0xE0){
		unsigned long temp = code;
		code = kbd_read_code();
		code = (code << 8) | temp;
	}
	printf("Acabei e ficou isto no OBF: 0x%x\n", code);
	return 0;
}

