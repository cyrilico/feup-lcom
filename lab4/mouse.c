#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "i8254.h"
#include "timer.h"
#include "mouse.h"

int hookid_mouse = 23;

int mouse_subscribe_int(int *hookid) {
	/*Variable that will hold return value in case of successful call, since sys_irq calls will modify hookid value*/
	int hookid_mouse_bit = BIT(*hookid);

	if(sys_irqsetpolicy(IRQ_MOUSE, IRQ_EXCLUSIVE | IRQ_REENABLE, hookid) != OK)
		return -1;

	if(sys_irqenable(hookid) != OK)
		return -1;

	return hookid_mouse_bit;
}

int mouse_unsubscribe_int(int *hookid) {
	if(sys_irqdisable(hookid) != OK)
		return -1;

	if(sys_irqrmpolicy(hookid) != OK)
		return -1;

	return 0;
}
