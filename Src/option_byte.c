/* option_byte.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 02.21.2020
 * Copyright (C) 2020
 *
 */

#include "main.h"
#include <stdio.h>

int print_option_bytes(void) {
  FLASH_OBProgramInitTypeDef config = {0};
  HAL_FLASHEx_OBGetConfig(&config);
  printf("USERConfig = %08x\n\r",(int) config.USERConfig);
  return(0);
}

  
  

