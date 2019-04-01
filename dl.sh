#!/bin/sh

# This command assumes that openocd is running - use "make gdb" to start. 
echo "load_image ./build/lm_application.elf" | telnet localhost 4444

return 0


