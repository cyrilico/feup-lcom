#include "test5.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"

void *test_init(unsigned short mode, unsigned short delay) {
	struct reg86u r;
	r.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = 1<<14|0x105; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;
	
	if( sys_int86(&r) != OK ) {
	printf("set_vbe_mode: sys_int86() failed \n");
	return 1;
	}
	
	timer_test_int(delay);
	return vg_exit(); //return to text mode
}


int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {
	
	/* To be completed */
	
}

int test_line(unsigned short xi, unsigned short yi, 
		           unsigned short xf, unsigned short yf, unsigned long color) {
	
	/* To be completed */
	
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
	
