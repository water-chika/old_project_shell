DESTDIR=
PREFIX=$$HOME/.local/bin
all: sh

sh : sh.c
	gcc sh.c -o sh -lreadline

install : all
	install sh ${DESTDIR}${PREFIX}

