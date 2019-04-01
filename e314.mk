# E314 Local targets

download: $(BUILD_DIR)/$(TARGET).elf
	./dl.sh 

etags:                                                                          
	find . -type f -iname "*.[ch]" | xargs etags --append

gdb:
	cp gdbinit ./build/.gdbinit
	openocd -f board/st_nucleo_l4.cfg 

reset:
	st-flash reset

