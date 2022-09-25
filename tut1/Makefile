PROG = tut1
SRC = ${PROG}.c
OBJ = ${SRC:.c=.o}


CC = gcc
CFLAGS = -I/usr/include/GL -I/usr/include/GLFW
LIBS = -lGL -lGLU -lGLEW -lEGL -lGL -lGLU -lOpenGL -lglfw -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor


%.o: %.c
	${CC} -c ${CFLAGS} $<

${PROG}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LIBS}


.PHONY: all install ${PROG}
