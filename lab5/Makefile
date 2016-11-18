COMPILER_TYPE= gnu
CC=gcc
PROG= lab5
SRCS= lab5.c test5.c vbe.c video_gr.c timer.c #more files to be added (handout talks about using keyboard and timer)
CCFLAGS= -Wall
DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys
LDFLAGS+= -L .
MAN=
BINDIR?= /usr/sbin
.include <bsd.prog.mk>
.include <bsd.gcc.mk>
