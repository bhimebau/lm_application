CC = gcc
OBJS = main.o interpreter.o ueac.o lla.o lla_function_definition.o conversion.o
OUT = ueac_interpreter
CFLAGS = -Wall -DLINUX
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
	./ueac_interpreter < commands.txt

