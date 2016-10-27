#include "i8254.h"

.global _kbd_read_code_ASM

.text

_kbd_read_code_ASM:

	push %bl /* convention states ebx is reserved */
	xorb %al, %al
NEXT:
	/*loop NTRIES times */
	cmpb %al, $NTRIES
	jmpe MAX_TRIES
		inb $STAT_REG, %bl /*assuming it returns OK, bl = value read from KBD */

		/* Check OBF content */
		andb $OBF, %bl
		cmpb $0, %bl
		je OBF_EMPTY
			inb $OUT_BUF, %cl /* cl = 'data' */
			/* Test parity error */
			testb $PARITY, %cl
			cmp $0, %cl
			jne PRTY_TO_ERROR
			/* Test timeout error */
			testb $TIMEOUT, %cl
			cmp $0, %cl
			jne PRTY_TO_ERROR
			movb %cl, %al /* return 'data' */
			jmp END
		PRTY_TO_ERROR:
			movb $-1, %al /* return -1*/
			jmp END
		OBF_EMPTY:
			/*WAIT 20ms*/
			incb %al
			jmp NEXT
	MAX_TRIES:
	movb $-1, %al
	END:
	pop ebx
	ret

