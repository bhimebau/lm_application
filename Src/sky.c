/* sky.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.17.2020
 * Copyright (C) 2020
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "main.h"
#include "skydata.h"
#include "power.h"
#include "sky.h"

#define MAX_ARGS 3

extern DAC_HandleTypeDef hdac1;

void sky_command(char *arguments) {
  float magvalue;
  int magvalue_int;
  int16_t dacval;
  char * endptr;
  char * argv[MAX_ARGS];
  char *p = arguments;
  int i;
  long value, range;

  if (!arguments) {
    /* HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(uint32_t) 0); */
    /* HAL_DAC_DeInit(&hdac1); */
    __HAL_DAC_DISABLE(&hdac1,DAC_CHANNEL_2);
    power_lock_enable = 0;
    sensor_power(POWER_OFF);
    printf("OK\n\r");
  }
  else {
    // Zero the arguments array 
    for (i=0;i<MAX_ARGS;i++) {
      argv[i] = (char *) 0;
    }
    argv[0] = p;
    i = 1;
    // locate the other arguments
    while ((*p) && (i < MAX_ARGS)){
      if (*p==',') {
        *p=0;
        argv[i++] = p+1;
      }
      p++;
    }
    if (!strcmp(argv[0],"raw")) {
      value = strtol(argv[1],NULL,10);
      range = strtol(argv[2],NULL,10);
      //      printf("%d %d\n\r",(int)value,(int)range);
      if (hdac1.State ==  HAL_DAC_STATE_RESET) {
        // DAC is uninitialized or disabled, need to enable
        MX_DAC1_Init();
        HAL_DAC_Start(&hdac1,DAC_CHANNEL_2);
      }
      __HAL_DAC_ENABLE(&hdac1,DAC_CHANNEL_2);
      sensor_power(POWER_ON);
      power_lock_enable = 1;
      sky_set_range((int32_t)range);
      HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(uint32_t) value);
      printf("OK\n\r");
    }
    else if (!strcmp(argv[0],"table")) {
      magvalue = strtof(argv[1],&endptr);
      magvalue_int  = (int) roundf(magvalue*10);
      dacval = sky2dac((uint32_t) magvalue_int);
      if (dacval < 0) {
        printf("Sky Value outside of range %2.1f to %2.1f\n\r",((float)BRIGHTEST_VALUE_ALLOWED)/10.0,((float)DARKEST_VALUE_ALLOWED)/10.0);
        printf("NOK\n\r");
        return;
      }
      else {
        //        printf("The magvalue is %d\n\r", magvalue_int);
        //        printf("The dacval is 0x%04x\n\r", (unsigned int) dacval);
        sky_set_range(dacval>>12);
        dacval &= 0x0FFF;
        //        printf("The dacval is 0x%04x\n\r", (unsigned int) dacval);
        
        if (hdac1.State ==  HAL_DAC_STATE_RESET) {
          // DAC is uninitialized or disabled, need to enable
          MX_DAC1_Init();
          HAL_DAC_Start(&hdac1,DAC_CHANNEL_2);
        }
        __HAL_DAC_ENABLE(&hdac1,DAC_CHANNEL_2);
        sensor_power(POWER_ON);
        power_lock_enable = 1;
        HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2,DAC_ALIGN_12B_R,dacval);
        printf("OK\n\r");
      }
    }
    else {
      printf("NOK\n\r");
    }
    
  }
}

uint32_t sky2dac(uint32_t sky_index) {
  // Make sure that the index is within the array 
  if ((sky_index > DARKEST_VALUE_ALLOWED) ||
      (sky_index < BRIGHTEST_VALUE_ALLOWED)) {
    return (-1);
  }
  //  printf("The index is: %ld\n\r", (sky_index - BRIGHTEST_VALUE_ALLOWED));
  return (skydata[sky_index -BRIGHTEST_VALUE_ALLOWED]);
}

uint32_t sky_set_range(int32_t range) {
  /* 
     Takes a range number and sets the led current source shunt. 
     -1 = All shunts off
     0 = 316 ohm
     1 = 1k 
     2 = 10k
     3 = 46.4k
   */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  //  printf("Resistor range = %d\n\r",(int)range);
  switch (range) {
  case -1:
    GPIO_InitStruct.Pin = irange_0_Pin|irange_0_Pin|irange_2_Pin|irange_3_Pin; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    break;
  case 0:
    GPIO_InitStruct.Pin = irange_1_Pin|irange_2_Pin|irange_3_Pin; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = irange_0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    break;
  case 1:
    GPIO_InitStruct.Pin = irange_0_Pin|irange_2_Pin|irange_3_Pin; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = irange_1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    break;
  case 2:
    GPIO_InitStruct.Pin = irange_0_Pin|irange_1_Pin|irange_3_Pin; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = irange_2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    break;
  case 3:
    GPIO_InitStruct.Pin = irange_0_Pin|irange_1_Pin|irange_2_Pin; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = irange_3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    break;
  default:
    GPIO_InitStruct.Pin = irange_0_Pin|irange_0_Pin|irange_2_Pin|irange_3_Pin; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    break;
  }
  return(0);
}

