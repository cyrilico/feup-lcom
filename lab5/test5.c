#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
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
	return 0;
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
	
