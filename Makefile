include config.mk

all: build

build:
	${CC} -o ${OUT} ${CPPFLAGS} ${LIBS} ${SRC}

install: build
	cp ${OUT} /usr/local/bin -f
	chmod 111 /usr/local/bin/${OUT}

clean:
	rm -f ${OUT}

uninstall:
	rm -f /usr/local/bin/${OUT}
