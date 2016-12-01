COMPILER_TYPE= gnu
CC=gcc
PROG= infinity
SRCS= main.c vbe.c video_gr.c timer.c keyboard.c mouse.c kbd_read_code_ASM.S sprite.c bitmap.c game.c
CCFLAGS= -Wall
DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys
LDFLAGS+= -L .
MAN=
BINDIR?= /usr/sbin
.include <bsd.prog.mk>
.include <bsd.gcc.mk>
