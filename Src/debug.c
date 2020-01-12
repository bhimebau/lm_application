/* debug.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.10.2020
 * Copyright (C) 2020
 *
 */

#include "main.h"
#include <stm32l4xx_ll_lpuart.h>
#include <stdio.h>
#include <string.h>

void debug_command(char *arguments) {
  if (arguments) {
    if (!strcmp(arguments,"enable")) {
      printf("Stop Mode Debug Enabled - keeps ETM active in stop to allow gdb connection when processor in stop mode\n\r");
      HAL_DBGMCU_EnableDBGStopMode();
      
    }
    else if (!strcmp(arguments,"disable")){
      printf("Stop Mode Debug disabled - shuts down ETM hardware in stop mode to save power. Blocks gdb connection\n\r");
      HAL_DBGMCU_DisableDBGStopMode();
    }
    else {
      printf("Argument Error: must be enable or disable - received %s\n\r",arguments);
    }
  }
  else {
    printf("Argument Error: no argument passed, must be enable or disable\n\r");
  }
}

