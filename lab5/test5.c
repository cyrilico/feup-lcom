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

void *test_init(unsigned short mode, unsigned short delay) {
	struct reg86u r;
	r.u.w.ax = VBE_CALL | VBE_SET_MODE;
	r.u.w.bx = BIT(14) | mode; // set bit 14: linear framebuffer
	r.u.b.intno = INT10;
	
	if( sys_int86(&r) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}
	
	vbe_mode_info_t current_mode_information;
	if(vbe_get_mode_info(mode, &current_mode_information) != OK)
		return NULL;

	timer_test_int(delay);
	vg_exit();
	printf("Voltei e o endereco fisico corresponde a: 0x%X\n", current_mode_information.PhysBasePtr);
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
			 * to seem kind of 'broken'*/
			x_initial += 1;
			y_initial += slope;
		}
	}
	kbd_scan_loop(0);
	vg_exit();
	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {
	
	/* To be completed */
	
}	

int test_move(unsigned short xi, unsigned short yi, char *xpm[], 
				unsigned short hor, short delta, unsigned short time) {
	
	/* To be completed */
	
}					

int test_controller() {
	
	/* To be completed */
	
}					
	
