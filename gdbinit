# Reset the target
define reset
 monitor reset halt
end

target extended-remote :3333
load
b main
c
