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
#include "led.h"

void lof_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    //    HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_RESET);
    led_off();
    printf("OK\n\r");
  }
}

void lon_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    //    HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_SET);
    led_on();
    printf("OK\n\r");
  }
}

void led_on(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = led_out_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(led_out_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_SET);
}

void led_off(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = led_out_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(led_out_GPIO_Port, &GPIO_InitStruct);
}

