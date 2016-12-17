#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "timer.h"
#include "i8254.h"

unsigned long interrupt_counter = 0;
int hookid = 15;

int timer_subscribe_int(void) {
	/*Bit specified in hookid to be returned by function if both calls are successful
	 (variable needed because hookid will be modified in case of successful call)*/
	int hookid_bit = BIT(hookid);

	switch(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hookid)) {
		case 0: //Success in setpolicy call
			if(sys_irqenable(&hookid) == OK) //Success in enable call
				return hookid_bit;
		default: //Unsuccessful call, no need to do enable call
			return -1;
}

	//Success in enable call
	return hookid_bit;
}

int timer_unsubscribe_int() {
	switch(sys_irqdisable( &hookid)) {
		case 0: //Success in disable call
			if(sys_irqrmpolicy(&hookid) == OK) //Success in unsubscribe call
				return 0;
		default: //Unsuccessful call, no need to do unsubscribe call
			return -1;
	}
}
