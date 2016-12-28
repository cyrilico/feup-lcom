#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "rtcmacro.h"
#include "rtc.h"

int hookid_rtc = 30;

int rtc_subscribe_int(){
	if(sys_irqsetpolicy(IRQ_RTC, IRQ_EXCLUSIVE | IRQ_REENABLE, &hookid_rtc) != OK)
		return -1;

	if(sys_irqenable(&hookid_rtc) != OK)
		return -1;

	return 0;
}


int rtc_unsubscribe_int(){
	if(sys_irqdisable(&hookid_rtc) != OK)
		return -1;

	if(sys_irqrmpolicy(&hookid_rtc) != OK)
		return -1;

	return 0;
}
