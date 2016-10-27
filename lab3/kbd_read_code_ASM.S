#include "i8254.h"

.global _kbd_read_code_ASM

.text

_kbd_read_code_ASM:

		xorl %eax, %eax
		mov $NTRIES, %cx
	NEXT:
		inb $STAT_REG /*assuming it returns OK, al = status read from KBD */
		testb $OBF, %al
		jz OBF_EMPTY
		testb $PARITY, %al
		jnz PRTY_TO_ERROR
		testb $TIMEOUT, %al
		jnz PRTY_TO_ERROR
		inb $OUT_BUF /* al = 'data' */
		jmp END
	PRTY_TO_ERROR:
		movl $-1, %eax
		jmp END
	OBF_EMPTY:
		/*WAIT 20ms*/
		loopnz NEXT
		movl $-1, %eax
END:	ret

