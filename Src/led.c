/* led.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 08. 2.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <stdio.h>

void lof_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_RESET);
    printf("OK\n\r");
  }
}

void lon_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_SET);
    printf("OK\n\r");
  }
}
