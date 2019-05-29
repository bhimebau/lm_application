CC = gcc
OBJS = main.o conversion.o
OUT = conversion_functions
CFLAGS = -Wall
LIBS = 

all: etags ${OUT} test

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OUT): ${OBJS}
	$(CC) -o $@ ${OBJS} ${LIBS}
clean: 
	rm -f ${OUT} *.o

etags: 
	etags *.c *.h

test: 
	./conversion_functions


