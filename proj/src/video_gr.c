#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>

#include "video_gr.h"
#include "vbe.h"
#include "video.h"
#include "lmlib.h"
#include "utils.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xE000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}

void* vg_init(unsigned short mode){
	lm_init();
	int r1;
	struct mem_range mr;
	unsigned int vram_base;
	unsigned int vram_size;

	struct reg86u r;
	r.u.w.ax = VBE_CALL | VBE_SET_MODE; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = BIT(14) | mode; // set bit 14: linear framebuffer
	r.u.b.intno = INT10;

	if( sys_int86(&r) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}

	vbe_mode_info_t current_mode_information;
	if(vbe_get_mode_info(mode, &current_mode_information) != OK)
			return NULL;

	vram_base = current_mode_information.PhysBasePtr;
	h_res = current_mode_information.XResolution;
	v_res = current_mode_information.YResolution;
	bits_per_pixel = current_mode_information.BitsPerPixel;
	vram_size = h_res*v_res*bits_per_pixel/8;

	/* Allow memory mapping */
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;

	if( OK != (r1 = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r1);
	/* Map memory */
	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
	if(video_mem == MAP_FAILED)
		panic("couldn’t map video memory");

	printf("Consegui %x \n", video_mem);
	return (void *)video_mem;
}


int vg_fill_pixel(unsigned int x, unsigned int y, unsigned long color) {
	if(x > h_res || y > v_res)
		return -1;
	char* position = video_mem + (y*h_res + x)*bits_per_pixel/8;
	int i = 0;
	while(i++ < bits_per_pixel/8)
		*(position++) = color;
	return 0;
}


int vg_fill_screen(unsigned int xi, unsigned int yi, unsigned int width, unsigned int height, unsigned long color)  {
	unsigned int x = xi;
	unsigned int y = yi;

	while(y < yi+height) {
		while(x < xi+width) {
			if(vg_fill_pixel(x++, y, color) != OK){
				vg_exit();
				return -1;
			}
		}
		y++;
		x = xi;
	}
	return 0;
}

int vg_draw_sprite(Sprite* s){
	int initial_x = s->x;
	int initial_y = s->y;
	int final_x = s->width + s->x;
	int final_y = s->height + s->y;

	unsigned int index = 0; //To access elements in color array
	while(initial_y < final_y) {
		while(initial_x < final_x){
			if(vg_fill_pixel(initial_x++, initial_y, s->map[index++]) != OK){
				vg_exit();
				return -1;
			}
		}
		initial_x = s->x;
		initial_y++;
	}
	return 0;
}

int vg_move_sprite(Sprite* s, float* x_cumulative, float* y_cumulative) {
	if(vg_fill_screen(s->x,s->y,s->width,s->height,BLACK) != OK)
		return -1;

	double trash; //Store useless information (needed as parameter for modf function)

	s->y += s->yspeed;
	*y_cumulative += modf(s->yspeed, &trash); //modf(arg1,arg2) - returns decimal part of arg1 and stores integer part in arg2
	if(abs(*y_cumulative) >= 1){ //Compensate for cumulative decimal part left behind
	            *y_cumulative += (*y_cumulative > 0 ? -1 : 1);
	            s->y += (*y_cumulative > 0 ? 1 : -1);
	}

	s->x += s->xspeed;
	*x_cumulative += modf(s->xspeed, &trash);
	if(abs(*x_cumulative) >= 1){ //Compensate for cumulative decimal part left behind
	            *x_cumulative += (*x_cumulative > 0 ? -1 : 1);
	            s->x += (*x_cumulative > 0 ? 1 : -1);
	}

	if(vg_draw_sprite(s) != OK)
			return -1;
	return 0;
}

unsigned int vg_get_h_res(){
	return h_res;
}

unsigned int vg_get_v_res(){
	return v_res;
}

unsigned int vg_get_bits_per_pixel(){
	return bits_per_pixel;
}

char* vg_get_video_mem(){
	return video_mem;
}
