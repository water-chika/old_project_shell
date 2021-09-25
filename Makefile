DESTDIR=
PREFIX=$$HOME/.local/bin
all: sh

sh : sh.c
	gcc sh.c -o sh

install : all
	install sh ${DESTDIR}${PREFIX}

