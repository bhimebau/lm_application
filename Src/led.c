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
#include <stdlib.h>
#include "led.h"
#include "power.h"

extern DAC_HandleTypeDef hdac1;
void MX_DAC1_Init(void);

void led_command(char *arguments) {
  uint32_t value = 0;
  uint32_t range = 0;
  char * p;
  if (arguments) {
    value = (uint32_t) strtol(arguments,&p,10);
    p++;
    range = (uint32_t) strtol(p,NULL,10);
    if (led_drive(value,range) == -1) {
      printf("NOK from -1\n\r");
    }
    else {
      /* printf("%d %d\n\r",(int) value, (int) range); */
      printf("OK\n\r");
    }
  }
  else {
    printf("Led Drive Command - expects 2 arguments\n\r");
    printf("value: 0-4095 integer that specifies a DAC voltage between 0V-2V\n\r");
    printf("range: 0-3 integer that specifies the current range for the V-I converter.\n\r");
    printf("NOK\n\r");
  }
}

// Function to set precisely drive the LED on the board
// Inputs:
//   value: 12-bit DAC value. If 0 is sent, the drive is disabled.
//   range: enables 1 of 4 range resistors. This allows for both very high
//          precision control of light output and range to extend to currents
//          for a brighter led. 
int led_drive(uint32_t value, uint32_t range) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  DAC_ChannelConfTypeDef sConfig = {0};
  
  if ((value > 0xFFF) | (range > 3)) {
    printf("%d %d\n\r",(int) value,(int) range);
    return (-1);
  }
  // If value is non-zero, then setup to select the appropriate range
  // resistor (irange_0 - irange_3). 
  if (value != 0) {
    /* if (hdac1.State == HAL_DAC_STATE_RESET) { */
    /*   printf("Starting the DAC\n\r"); */
    /*   HAL_DAC_Start(&hdac1,DAC_CHANNEL_2); */
    /* }     */
    HAL_DBGMCU_EnableDBGStopMode(); // The debugger seems to need to be enabled 
    sensor_power(POWER_ON);
    if (HAL_DAC_Init(&hdac1) != HAL_OK) {
      Error_Handler();
    }
    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    if (HAL_DACEx_SelfCalibrate(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK) {
      Error_Handler();
    }
    HAL_DAC_Start(&hdac1,DAC_CHANNEL_2);
    /* printf("About to set the channel to %d\n\r",(int) value); */
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2,DAC_ALIGN_12B_R,value); 
    switch (range) {
    case 0:
      // Float all of the other control pins
      GPIO_InitStruct.Pin =
        irange_1_Pin|
        irange_2_Pin|
        irange_3_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      // Change pin 0 to an push-pull output set to low. 
      HAL_GPIO_WritePin(GPIOB,irange_0_Pin, GPIO_PIN_RESET);
      GPIO_InitStruct.Pin = irange_0_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
      break;
    case 1:
      // Float all of the other control pins
      GPIO_InitStruct.Pin =
        irange_0_Pin|
        irange_2_Pin|
        irange_3_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      // Change pin 0 to an push-pull output set to low. 
      HAL_GPIO_WritePin(GPIOB,irange_1_Pin, GPIO_PIN_RESET);
      GPIO_InitStruct.Pin = irange_1_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
      break;
    case 2:
      // Float all of the other control pins
      GPIO_InitStruct.Pin =
        irange_0_Pin|
        irange_1_Pin|
        irange_3_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      // Change pin 0 to an push-pull output set to low. 
      HAL_GPIO_WritePin(GPIOB,irange_2_Pin, GPIO_PIN_RESET);
      GPIO_InitStruct.Pin = irange_2_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
      break;
    case 3:
      // Float all of the other control pins
      GPIO_InitStruct.Pin =
        irange_0_Pin|
        irange_1_Pin|
        irange_2_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      // Change pin 0 to an push-pull output set to low. 
      HAL_GPIO_WritePin(GPIOB,irange_3_Pin, GPIO_PIN_RESET);
      GPIO_InitStruct.Pin = irange_3_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
      break;
    default:
      return (-1);
    }
  }
  // value is 0, this indicates that the drive should be off.
  // in this case, turn off all of the resistors. 
  else {
    HAL_DBGMCU_DisableDBGStopMode();
    sensor_power(POWER_OFF);
    HAL_DAC_Stop(&hdac1,DAC_CHANNEL_2);
    HAL_DAC_DeInit(&hdac1);
    GPIO_InitStruct.Pin =
      irange_0_Pin|
      irange_1_Pin|
      irange_2_Pin|
      irange_3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
  return (0);
}



