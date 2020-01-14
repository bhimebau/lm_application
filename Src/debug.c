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
#include "debug.h"
#include <stm32l4xx_ll_lpuart.h>
#include <stdio.h>
#include <string.h>

void debug_command(char *arguments) {
  if (arguments) {
    if (!strcmp(arguments,"enable")) {
      printf("Stop Mode Debug Enabled - keeps ETM active in stop to allow gdb connection when processor in stop mode\n\r");
      HAL_DBGMCU_EnableDBGStopMode();
      // debug_enable();
    }
    else if (!strcmp(arguments,"disable")){
      printf("Stop Mode Debug disabled - shuts down ETM hardware in stop mode to save power. Blocks gdb connection\n\r");
      HAL_DBGMCU_DisableDBGStopMode();
      //  debug_disable();
    }
    else {
      printf("Argument Error: must be enable or disable - received %s\n\r",arguments);
    }
  }
  else {
    printf("Argument Error: no argument passed, must be enable or disable\n\r");
  }
}

void debug_enable(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // PA13 and PA14
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SWJ;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // PB3 
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_DBGMCU_EnableDBGStopMode();
}

void debug_disable(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // PA13 and PA14
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SWJ;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // PB3 
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_DBGMCU_DisableDBGStopMode();
}



