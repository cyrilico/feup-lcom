#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>
#include "test5.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "video.h"
#include "sprite.h"
#include "read_xpm.h"
#include "i8042.h"

void *test_init(unsigned short mode, unsigned short delay) {
	/* In the handout it states not to map memory in this function, but is it only DIRECTLY in this function? Assumed so, so substituted code below for
	 * vg_init + vbe_get_mode_info calls */
	/*struct reg86u r;
	r.u.w.ax = VBE_CALL | VBE_SET_MODE;
	r.u.w.bx = BIT(14) | mode; // set bit 14: linear framebuffer
	r.u.b.intno = INT10;
	
	if( sys_int86(&r) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}
	
	vbe_mode_info_t current_mode_information;
	if(vbe_get_mode_info(mode, &current_mode_information) != OK)
		return NULL;*/


	void* result = vg_init(mode);
	vbe_mode_info_t current_mode_information;
	if(vbe_get_mode_info(mode, &current_mode_information) != OK)
		return NULL;
	timer_test_int(delay);
	vg_exit();
	printf("Back to text mode, VRAM physical address is: 0x%X\n", current_mode_information.PhysBasePtr);
	return 0;
}


int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {
	if(vg_init(MODE105) == NULL)
		return -1;
	unsigned int xi = x;
	/*If x+size goes out of memory range, limit it to the monitor's border and draw figure partially (instead of exiting without drawing anything)*/
	unsigned int xf = (x+size > vg_get_h_res() ? vg_get_h_res() : x+size);
	unsigned int yi = y;
	/*If y+size goes out of memory range, limit it to the monitor's border and draw figure partially (instead of exiting without drawing anything)*/
	unsigned int yf = (y+size > vg_get_v_res() ? vg_get_v_res() : y+size);
	while(yi < yf){
		while(xi < xf){
			if(vg_fill_pixel(xi++, yi, color) != OK){
				vg_exit();
				return -1;
			}
		}
		xi = x;
		yi++;
	}
	kbd_scan_loop(0);
	vg_exit();
	return 0;
}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color) {
	if(vg_init(MODE105) == NULL)
			return -1;
	/*If values go over the video memory, limit them to the border to avoid writing on unknown memory*/
	xi = (xi > vg_get_h_res() ? vg_get_h_res() : xi);
	yi = (yi > vg_get_v_res() ? vg_get_v_res() : yi);
	xf = (xf > vg_get_h_res() ? vg_get_h_res() : xf);
	yf = (yf > vg_get_v_res() ? vg_get_v_res() : yf);
	float x_initial;
	float x_final;
	float y_initial;
	float y_final;
	if(xi == xf){ //Vertical line -> infinite slope (special case)
		//Line is always drown going down
		y_initial = (yi < yf ? yi : yf);
		y_final = (yi < yf ? yf : yi);
		while(y_initial < y_final){
			if(vg_fill_pixel(xi, y_initial, color) != OK){
				vg_exit();
				return -1;
			}
			y_initial += 1;
		}
	}
	else{ //Non-vertical line -> slope with a real, determinable value (general case)
		//Line is always drown left to right
		x_initial = (xi < xf ? xi : xf);
		x_final = (xi < xf ? xf : xi);
		y_initial = (x_initial == xi ? yi : yf);
		y_final = (x_final == xf ? yf : yi);
		float slope = (y_final-y_initial)/(x_final-x_initial);
		while(x_initial < x_final){
			if(vg_fill_pixel(x_initial, y_initial, color) != OK){
				vg_exit();
				return -1;
			}
			/*In each iteration, take the current point and translate it using the vector (x,y) = (1,slope)
			 * Of course, y value will be rounded when used as argument for vg_fill_pixel(), which will cause non-horizontal lines with reduced slope
			 * or a big slope to seem kind of 'broken'*/
			x_initial += 1;
			y_initial += slope;
		}
	}
	kbd_scan_loop(0);
	vg_exit();
	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {
	if(vg_init(MODE105) == NULL)
			return -1;
	if(xi > vg_get_h_res() || yi > vg_get_v_res()){
		vg_exit();
		printf("Invalid xi and/or yi\n");
		return -1;
	}

	Sprite* sp = create_sprite(xpm, xi, yi, 0, 0);
	if(sp == NULL){
		vg_exit();
		printf("Error in reading xpm\n");
		return -1;
	}

	if(sp->width + xi > vg_get_h_res() || sp->height + yi > vg_get_v_res()){ //This and not readjustment of variables because color array is sequential so pretty hard to cover the case
		vg_exit();
		printf("Upper left corner coordinates are too tight! Poor xpm can't show himself fully.\n");
		return -1;
	}

	if(vg_draw_sprite(sp) != OK)
		return -1;

	kbd_scan_loop(0);
	vg_exit();
	return 0;
}	

int test_move(unsigned short xi, unsigned short yi, char *xpm[], 
				unsigned short hor, short delta, unsigned short time) {
	if(vg_init(MODE105) == NULL)
			return -1;

	float speed = delta/(time*60.0);

	Sprite* sp;
	if(hor != 0)
		sp = create_sprite(xpm, xi, yi, speed, 0);
	else
		 sp = create_sprite(xpm, xi, yi, 0, speed);

	if(sp == NULL){
		vg_exit();
		return -1;
	}

	int irq_timer = timer_subscribe_int();
	if(irq_timer == -1) //Failed subscription
		return -1;

	int irq_keyboard = kbd_subscribe_int();
	if(irq_keyboard == -1) //Failed subscription
		return -1;

	int r;
	int ipc_status;
	message msg;

	unsigned short counter = 0;
	int read_again = 0;
	unsigned long code = 0;
	unsigned long code_aux = 0;

	if(vg_draw_sprite(sp) != OK)
		return -1;

	int early_exit = 0; //Exited before movement completed? 0 if not, 1 if yes

	while(counter/60 < time && code != ESC_BREAK) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_timer) { /* subscribed interrupt */
					counter++;
					if(vg_move_sprite(sp) != OK)
						return -1;
				}
				if(msg.NOTIFY_ARG & irq_keyboard){
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
					if(code == ESC_BREAK)
						early_exit = 1;
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}


	if(timer_unsubscribe_int() != OK || kbd_unsubscribe_int() != OK)
		return -1;

	if(!early_exit)
		kbd_scan_loop(0);

	vg_exit();
	return 0;
}					

int test_controller() {
	vbe_controller_info_t current_controller_information;
	if(vbe_get_controller_info(&current_controller_information) != OK){
		printf("Error acquiring controller information\n");
		return -1;
	}

	printf("Current controller capabilities:\n");
	printf("%s", current_controller_information.Capabilities[0] ? "DAC width is switchable to 8 bits per primary color\n" : "DAC is fixed width, with 6 bits per primary color\n");
	printf("%s", current_controller_information.Capabilities[1] ? "Controller is not VGA compatible\n" : "Controller is VGA compatible\n");
	printf("%s", current_controller_information.Capabilities[2] ? "When programming large blocks of information to the RAMDAC, use the blank bit in Function 09h\n" : "Normal RAMDAC Operation\n");
	
	printf("\n");
	printf("Current available modes, in hexadecimal:\n");
	unsigned int index = 0;
	while(((current_controller_information.Reserved[index+1]<<8) | current_controller_information.Reserved[index]) != 0xFFFF){
		printf("0x%x ", ((current_controller_information.Reserved[index+1]<<8) | current_controller_information.Reserved[index]));
		index += 2;
	}
	printf("\n\n");
	
	printf("Current total memory, in kB: %u\n", current_controller_information.TotalMemory*64);
	return 0;
}					
	
