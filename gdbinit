# Reset the target

define portb
  echo PORTB->IDR
  p/x GPIOB->IDR
end

define read_msi_pll
  p/x RCC->CR
  if (RCC->CR&0x00000004)
     echo MSI PLL Enabled\n
  else
     echo MSI PLL Disabled\n
  end
end

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



