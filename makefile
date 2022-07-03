CC=gcc
CFLAGS= -Wall -Werror

SEEsh.o: SEEsh.c
	${CC} ${CFLAGS} -o $@ $<

.PHONY = clean

clean:
	rm SEEsh.o SEEsh