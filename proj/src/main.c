/*#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
*/
#include "menu.h"

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
	char* double_buffer = (char*)(malloc(vg_get_window_size()));
	//Bitmap* buzz = loadBitmap(fullPath("buzzie.bmp"),vg_get_h_res()/2,vg_get_v_res()/2);
	Menu* menu = create_menu(double_buffer);
	//drawBitmap(background,vg_get_video_mem(),ALIGN_LEFT);
	//drawBitmap(buzz,vg_get_video_mem(),ALIGN_LEFT);
	//draw_mouse(mouse,vg_get_video_mem());
	int r;
	int ipc_status;
	message msg;

	unsigned short number_of_packets = 0; //number of full packets processed
	unsigned short counter = 0;
	long byte; //Auxiliar variable that will store each byte read

	if(mouse_write_byte(ENABLE_MOUSE_DATA_REPORTING) == -1)
			return -1;

	while(menu->state != DONE) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & menu->irq_mouse) { /* subscribed interrupt */
					sys_inb(OUT_BUF, &byte);
					if(counter == 0){
						if(byte & BIT(3)) //Valid first byte (or at least we hope so)
							menu->mouse->packet[counter++] = (unsigned char)byte;
						else{
							printf("Invalid first byte, trying again\n");
							continue;
						}
					}
					else{
						menu->mouse->packet[counter++] = (unsigned char)byte;
						if(counter > 2){
							mouse_print_packet(menu->mouse->packet);
							counter = 0;
							if(++number_of_packets != 1){
								update_menu(menu);
								draw_menu(menu);
							}
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
	}

	vg_exit();
	if(mouse_write_byte(DISABLE_MOUSE_DATA_REPORTING) == -1){
		printf("fodeu\n");
		return -1;
	}
	return mouse_unsubscribe_int();
}

