#include "rtcmacro.h"

.global _rtc_get
/* Two pointers to two different arrays because we're not using malloc to generate a new pointer each time we call it (like we'd probably do in C)
 * so consecutive calls (without treatment in-between, like we do in game.c) would result in loss of data resulting of the first call */
.global _result_date
.global _result_time

.data
_result_date: .long 0,0,0
_result_time: .long 0,0,0

.text

_rtc_get:
		/* Create stack frame */
		pushl %ebp
		movl %esp, %ebp
cycle:	cli /* Disable interrupts */
		movb $REGA, %al
		outb $RTC_ADDR_REG /* Send message to RTC controller stating that we want to access Register A */
		inb $RTC_DATA_REG /* Current Register A configuration is in al */
		sti /* (Re)enable interrupts */
		testb $REGA_UIP, %al
		jnz cycle /* If flag set, do not read date/time registers. Loop until it is zero */
		movl 8(%ebp), %ecx /* Move function argument to ecx to be tested */
		testl $1, %ecx /* ecx <> 0? If yes, user chose to read date. Otherwise, user chose to read time */
		jnz getdate
		movl $_result_time, %edx /* Move base address of wanted array to edx (to use with displacements later on; if tried to use $_result_time directly assembler was giving strange errors?!) */
		movb $SECREG, %al
		outb $RTC_ADDR_REG /* Send message to RTC controller stating that we want to access seconds register (similar mechanism for other 2)*/
		inb $RTC_DATA_REG /* Current value of seconds register is in al (similar mechanism for other 2)*/
		movb %al, 8(%edx) /* Current result_time content: ?,?,current_time_seconds (? instead of 0 because we never know if it has been called before)*/
		movb $MINREG, %al
		outb $RTC_ADDR_REG
		inb $RTC_DATA_REG
		movb %al, 4(%edx) /* Current result_time content: ?,current_time_minutes,current_time_seconds */
		movb $HOUREG, %al
		outb $RTC_ADDR_REG
		inb $RTC_DATA_REG
		movb %al, (%edx) /* Current result_time content: current_time_hours,current_time_minutes,current_time_seconds */
		jmp end
getdate:movl $_result_date, %edx /* Move base address of wanted array to edx (to use with displacements later on; if tried to use $_result_time directly assembler was giving strange errors?!) */
		movb $DAYREG, %al
		outb $RTC_ADDR_REG
		inb $RTC_DATA_REG
		movb %al, (%edx) /* Current result_date content: current_date_day,?,? */
		movb $MONREG, %al
		outb $RTC_ADDR_REG
		inb $RTC_DATA_REG
		movb %al, 4(%edx) /* Current result_date content: current_date_day,current_date_month,? */
		movb $YEAREG, %al
		outb $RTC_ADDR_REG
		inb $RTC_DATA_REG
		movb %al, 8(%edx) /* Current result_date content: current_date_day,current_date_month,current_date_year(0-99) */
end:	movl %edx, %eax /* Move pointer to first element of modified array to conventioned eax register */
		/* Dismantle stack frame */
		mov %ebp, %esp
		popl %ebp
		ret
