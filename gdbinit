# Reset the target

define reset
 monitor reset halt
end

define eraseall
  monitor flash erase_sector 0 0 127
  load
end 

define recon
  target extended-remote :3333
end

target extended-remote :3333
load
b main
c
