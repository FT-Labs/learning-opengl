PROG = triangle
SRC = ${PROG}.c log.c gl_shader.c
OBJ = ${SRC:.c=.o}

CFLAGS = -Wall -Wextra -O3 -I/usr/include/X11 -I/usr/include/GL
LDFLAGS = -lX11 -lGL -lGLEW -L/usr/X11/lib -lglfw

CC = gcc

all: ${PROG}

%.o: %.c
	${CC} -c ${CFLAGS} $<

${PROG}: ${OBJ}
	${CC} -o $@ ${LDFLAGS} ${OBJ}

.PHONY: all ${PROG}
