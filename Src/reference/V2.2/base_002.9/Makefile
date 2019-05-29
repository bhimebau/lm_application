### Makefile --- 
## 
## Filename: Makefile
## Description: 
## Author: Bryce Himebaugh
## Maintainer: 
## Created: Wed Nov  2 11:26:09 2005
## Version: 
## Last-Updated: Mon Nov 14 09:31:33 2005
##           By: Bryce Himebaugh
##     Update #: 10
## Keywords: 
## Compatibility: 
## 
######################################################################
## 
### Commentary: 
## 
## 
## 
######################################################################
## 
### Change log:
## 
## 
######################################################################
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; see the file COPYING.  If not, write to the
## Free Software Foundation, Inc., 59 Temple Place - Suite 330,
## Boston, MA 02111-1307, USA.

## 
######################################################################
## 
### Code:

NAME            = base_r002
OBJECTS         = main.o ueaclib.o calibrate.o external_flash.o ueac.o interpreter.o filter.o timer.o \
		  conversion.o global.o lla.o lla_function_definition.o

INCLUDES 	= ueaclib.h calibrate.h conversion.h vi_table.h
CPU             = msp430x169
ASFLAGS         = -mmcu=${CPU} -D_GNU_ASSEMBLER_ -nostartfiles -nostandardlibs
CFLAGS          = -mmcu=${CPU} -O2 -Wall -g 

#Switch the compiler (for the internal make rules)
CC              = msp430-gcc 
AS              = msp430-gcc

.PHONY: all FORCE clean download download-jtag 

all: ${NAME}.elf ${NAME}.lst tags 

download: download-jtag 
reset: reset-jtag

${NAME}.elf: ${OBJECTS}
	${CC} -mmcu=${CPU} -o $@ ${OBJECTS}

${NAME}.lst: ${NAME}.elf
	msp430-objdump -dSt $^ >$@

download-jtag: all
	msp430-jtag -ep ${NAME}.elf 

debug-jtag: all 
	msp430-jtag -eEpv -DDDDDDDD ${NAME}.elf 2>&1 | tee jtag_log

reset-jtag: all 
	msp430-jtag -r 

clean:
	rm -f ${NAME}.elf $(OBJECTS) ${NAME}.lst dox.log

${OBJECTS}: ${INCLUDES}

tags: 
	etags *.c *.h

doxygen: 
	doxygen Doxyfile 

dox_install:
	cp -r ./doc ~/.hyplan/ueac/uEACos
	chmod -R o+r  ~/.hyplan/ueac/uEACos
	find ~/.hyplan/ueac/uEACos/. -type d | xargs chmod o+x

######################################################################
### Makefile ends here
