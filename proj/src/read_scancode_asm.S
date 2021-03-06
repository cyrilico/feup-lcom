#include "i8042.h"

.global _read_scancode_asm

.text

_read_scancode_asm:

        movl $NTRIES, %ecx
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
        /* Wait 20 ms before retrying */
        /* micros_to_ticks(DELAY_US) */
        pushl $DELAY_US
        call _micros_to_ticks
        addl $4, %esp /* Clean stack after function as specified by convention */
        /* tickdelay(eax), with eax holding the value returned from previous call */
        pushl %eax
        call _tickdelay
        addl $4, %esp /* Clean stack (same reason as above) */
        loopnz NEXT
        movl $-1, %eax
END:    ret
