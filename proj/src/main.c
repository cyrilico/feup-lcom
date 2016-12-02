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
#include "mouse.h"

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
	int window_size = vg_get_h_res()*vg_get_v_res()*2;
	char* double_buffer = (char*)(malloc(window_size));
	Bitmap* background = loadBitmap(fullPath("game_background.bmp"),0,0);
	Bitmap* buzz = loadBitmap(fullPath("buzzie.bmp"),0,vg_get_v_res()/2);
	Mouse* mouse = create_mouse();
	buzz->x = vg_get_h_res()-buzz->bitmapInfoHeader.width;
	drawBitmap(background,vg_get_video_mem(),ALIGN_LEFT);
	//drawBitmap(buzz,vg_get_video_mem(),ALIGN_LEFT);
	draw_mouse(mouse,vg_get_video_mem());
	int r;
	int irq_set = mouse_subscribe_int();

	if(irq_set == -1) //Failed subscription
		return -1;

	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	unsigned long byte; //Auxiliar variable that will store each byte read

	if(mouse_write_byte(ENABLE_MOUSE_DATA_REPORTING) == -1)
		return -1;

	while(number_of_packets < 500) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					sys_inb(OUT_BUF, &byte);
					if(counter == 0){
						if(byte & BIT(3)) //Valid first byte (or at least we hope so)
							mouse->packet[counter++] = (unsigned char)byte;
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						mouse->packet[counter++] = (unsigned char)byte;
						if(counter > 2){
							mouse_print_packet(mouse->packet);
							update_mouse(mouse);
							counter = 0;
							number_of_packets++;
							drawBitmap(background,vg_get_video_mem(),ALIGN_LEFT);
							//drawBitmap(buzz,vg_get_video_mem(),ALIGN_LEFT);
							draw_mouse(mouse,vg_get_video_mem());
						}
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
		//tickdelay(micros_to_ticks(DELAY_US*5));
	}

	if(mouse_write_byte(DISABLE_MOUSE_DATA_REPORTING) == -1)
		return -1;
	vg_exit();
	return mouse_unsubscribe_int();
}

