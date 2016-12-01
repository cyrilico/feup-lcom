COMPILER_TYPE= gnu
CC=gcc
PROG= proj
SRCS= main.c vbe.c video_gr.c timer.c keyboard.c mouse.c sprite.c bitmap.c utils.c
CCFLAGS= -Wall
DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys
LDFLAGS+= -L .
MAN=
BINDIR?= /usr/sbin
.include <bsd.prog.mk>
.include <bsd.gcc.mk>
