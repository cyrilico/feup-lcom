#include "rtc.h"

int hookid_rtc = 30;

int rtc_subscribe_int(){
	/*Variable that will hold return value in case of successful call, since sys_irq calls will modify hookid value*/
	int hookid_rtc_bit = BIT(hookid_rtc);

	unsigned long config = BIT(4)|BIT(2)|BIT(1); //Update ended interrupts (will use UIP of REGA to control), values in binary, 24 hour format
	sys_outb(RTC_ADDR_REG, REGB);
	sys_outb(RTC_DATA_REG, config);

	if(sys_irqsetpolicy(IRQ_RTC, IRQ_EXCLUSIVE | IRQ_REENABLE, &hookid_rtc) != OK)
		return -1;

	if(sys_irqenable(&hookid_rtc) != OK)
		return -1;

	return hookid_rtc_bit;
}


int rtc_unsubscribe_int(){
	if(sys_irqdisable(&hookid_rtc) != OK)
		return -1;

	if(sys_irqrmpolicy(&hookid_rtc) != OK)
		return -1;

	return 0;
}

unsigned long* rtc_get(int time_or_date){
	unsigned long* result = (unsigned long*)(malloc(3*sizeof(unsigned long)));
	unsigned long regauip = 0; //Wait for valid period to acess registers
	do{
		sys_outb(RTC_ADDR_REG, REGA);
		sys_inb(RTC_DATA_REG, &regauip);
	}while(regauip & REGA_UIP);

	/* Friendly reminder if you come back here later and try to change this: CAN'T USE ++ OPERATOR! Will fuck up return value
	 * (could always do a couple of prefix -- in the end but nah) */
	if(time_or_date == 0){ //Read time
		sys_outb(RTC_ADDR_REG, SECREG);
		sys_inb(RTC_DATA_REG, result+2);
		sys_outb(RTC_ADDR_REG, MINREG);
		sys_inb(RTC_DATA_REG, result+1);
		sys_outb(RTC_ADDR_REG, HOUREG);
		sys_inb(RTC_DATA_REG, result);
	}
	else{ //Read date
		sys_outb(RTC_ADDR_REG, DAYREG);
		sys_inb(RTC_DATA_REG, result);
		sys_outb(RTC_ADDR_REG, MONREG);
		sys_inb(RTC_DATA_REG, result+1);
		sys_outb(RTC_ADDR_REG, YEAREG);
		sys_inb(RTC_DATA_REG, result+2);
		*(result+2) += 2000; //Year is a number from 0-99 so it should give out something like 16 or 17 (corresponding to 2016 and 2017). Here we're adjusting the value
	}

	return result;
}


