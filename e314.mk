# E314 Local targets

download: $(BUILD_DIR)/$(TARGET).elf
	./dl.sh 

etags:
	rm -f TAGS
	find . -type f -iname "*.[ch]" | xargs etags --append

gdb: $(BUILD_DIR)/$(TARGET).elf
	cp gdbinit ./build/.gdbinit
#	openocd -s /l/arm2/isetools/tcl -f board/st_nucleo_l4.cfg 
#	openocd -s ~/nobackup/openocd/0.10.0-12-20190422-2015/scripts -f board/st_nucleo_l4.cfg 
#	openocd -s ~/install_fake/share/openocd/scripts -f board/st_nucleo_l4.cfg
	openocd -s ~/install_fake/share/openocd/scripts -f interface/stlink.cfg -f target/stm32l4x.cfg
reset:
	st-flash reset

