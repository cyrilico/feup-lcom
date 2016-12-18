#include "i8042.h"

.global _read_scancode_asm

.text

_read_scancode_asm:

		mov $NTRIES, %cx
	NEXT:
		xorl %eax, %eax
		inb $STAT_REG /* assuming it returns OK, al = status read from KBD */
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
		/* Wait 20 ms TO DO: Try to actually implement the 20ms waiting */
		loopnz NEXT
		movl $-1, %eax
END:	ret

