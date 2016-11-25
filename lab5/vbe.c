#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"
#include "video.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
	mmap_t temp;
	if(lm_alloc(sizeof(vbe_mode_info_t), &temp) == NULL)
		return -1;

	struct reg86u r;
	r.u.w.ax = VBE_CALL | VBE_GET_MODE_INFO;
	r.u.w.cx = mode;
	r.u.w.es = PB2BASE(temp.phys);
	r.u.w.di = PB2OFF(temp.phys);
	r.u.b.intno = INT10;
	if( sys_int86(&r) != OK ) {
		printf("vbe_get_mode_info: sys_int86() failed \n");
		return -1;
	}

	if(r.u.w.ax != SUCCESSFUL_VBE_CALL){
		printf("vbe_get_mode_info: ");
		if(r.u.b.ah == 1)
			printf("function call failed\n");
		else if(r.u.b.ah == 2)
			printf("function is not supported in current HW configuration\n");
		else if(r.u.b.ah == 3)
			printf("function is invalid in current video mode\n");
		return -1;
	}

	memcpy(vmi_p, temp.virtual, temp.size);
	lm_free(&temp);
	return 0;
}

int vbe_get_controller_info(vbe_controller_info_t *vci_p){
	lm_init();
	mmap_t temp;

	if(lm_alloc(sizeof(vbe_controller_info_t), &temp) == NULL)
		return -1;

	struct reg86u r;
	r.u.w.ax = VBE_CALL | VBE_GET_CONTROLLER_INFO;
	r.u.w.es = PB2BASE(temp.phys);
	r.u.w.di = PB2OFF(temp.phys);
	r.u.b.intno = INT10;
	if( sys_int86(&r) != OK ) {
		printf("vbe_get_mode_info: sys_int86() failed \n");
		return -1;
	}

	if(r.u.w.ax != SUCCESSFUL_VBE_CALL){
		printf("vbe_get_controller_info: ");
		if(r.u.b.ah == 1)
			printf("function call failed\n");
		else if(r.u.b.ah == 2)
			printf("function is not supported in current HW configuration\n");
		else if(r.u.b.ah == 3)
			printf("function is invalid in current video mode\n");
		return -1;
	}

	memcpy(vci_p, temp.virtual, temp.size);
	lm_free(&temp);
	return 0;
}


