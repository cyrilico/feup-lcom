COMPILER_TYPE= gnu
CC=gcc
PROG= proj
SRCS= bitmap.c dispatcher.c game.c keyboard.c main.c menu.c mouse.c obstacle.c player.c read_scancode_asm.S rtc.c score.c timer.c utils.c vbe.c video_gr.c
CCFLAGS= -Wall
DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys
LDFLAGS+= -L .
MAN=
BINDIR?= /usr/sbin
.include <bsd.prog.mk>
.include <bsd.gcc.mk>
