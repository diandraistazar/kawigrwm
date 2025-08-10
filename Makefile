include config.mk

all: build

build:
	${CC} -o ${OUT} ${CPPFLAGS} ${LIBS} ${SRC}
