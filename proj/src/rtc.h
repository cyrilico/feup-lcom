#pragma once

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#define BIT(n) (0x01<<(n))
#define IRQ_RTC 8
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define REGA 0xA
#define REGB 0xB
#define REGA_UIP BIT(7)
#define SECREG 0x00
#define MINREG 0x02
#define HOUREG 0x04
#define DAYREG 0x07
#define MONREG 0x08
#define YEAREG 0x09
#define CURRENT_TIME 0
#define CURRENT_DATE 1

int rtc_subscribe_int(); //does it need to return the BIT of the hookid? not sure until I integrate it in the dispatcher class (may need it even if it's only to clear interrupt from register C)
int rtc_unsubscribe_int();
unsigned long* rtc_get(int time_or_date);