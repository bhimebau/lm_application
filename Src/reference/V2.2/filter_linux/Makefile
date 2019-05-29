CC = gcc
OBJS = main.o filter.o
OUT = ueac_a2d_filter
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
	./ueac_a2d_filter

