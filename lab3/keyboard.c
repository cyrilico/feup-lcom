#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "keyboard.h"

int hookid_kbd = 10;

int kbd_subscribe_int(int *hookid) {
	/*Variable that will hold return value in case of successful call, since sys_irq calls will modify hookid value*/
	int hookid_kbd_bit = BIT(*hookid);

	if(sys_irqsetpolicy(IRQ_KBD, IRQ_EXCLUSIVE | IRQ_REENABLE, hookid) != OK)
		return -1;

	if(sys_irqenable(hookid) != OK)
		return -1;

	return hookid_kbd_bit;
}

int kbd_unsubscribe_int(int *hookid) {

	if(sys_irqdisable(hookid) != OK)
		return -1;

	if(sys_irqrmpolicy(hookid) != OK)
		return -1;

	return 0;
}

unsigned long kbd_read_code() {

	unsigned long st;
	unsigned long data;
	unsigned int counter = 0;

	while(counter <= NTRIES) {
		sys_inb(STAT_REG, &st);
		/* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if(st & OBF) {
			sys_inb(OUT_BUF, &data); /* assuming it returns OK */
			if ( (st &(PARITY | TIMEOUT)) == 0 )
				return data;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		counter++;
	}
}

int kbd_scan_loop() {

	int r;
	int irq_set = kbd_subscribe_int(&hookid_kbd);

	if(irq_set == -1) //Failed subscription
		return -1;

	int ipc_status;
	message msg;

	unsigned long code; //alterei isto

	while(code != ESC_BREAK) {
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					//Read individual codes
					code = kbd_read_code();

					if(code == -1)
						return -1;

					if(code & BIT(7))
						printf("Breakcode: 0x%x\n", code);
					else
						printf("Makecode: 0x%x\n", code);
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

}
