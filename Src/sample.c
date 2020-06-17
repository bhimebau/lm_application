/* sample.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.13.2020
 * Copyright (C) 2020
 *
 */

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "flash.h"
#include "rtc.h"
#include "battery.h"
#include "temperature.h"
#include "tsl237.h"
#include "sample.h"
#include "power.h"

extern flash_status_t fs;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

void sample_command(char * arguments) {
  if (arguments) {
    printf("NOK\n\r");
    return;
  }
  sample();
  printf("OK\n\r");
}

void sample(void) {
  uint32_t raw;
  sensor_power(POWER_ON);
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_IC_Init(&htim2);
  HAL_Delay(3);
  raw = tsl237_readsensor();

  HAL_ADC_Init(&hadc1);
  // Calibrate the A2D
  while (HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED) != HAL_OK); // must be done on each init of the converter. 
  //  write_sensor_data(&fs,read_vrefint(),read_temp(),tsl237_readsensor());
  //  printf("battery value=%d\n\r",(int)((read_battery()*488*2)/1000)); 
  //write_sensor_data(&fs,read_battery(),read_temp(),tsl237_readsensor());
  write_sensor_data(&fs,read_battery(),read_temp(),raw);
  HAL_ADC_DeInit(&hadc1);
  HAL_TIM_Base_DeInit(&htim2);
  HAL_TIM_IC_DeInit(&htim2);
  sensor_power(POWER_OFF);
}


// This function is used in the calibration process. When a write occurs, the 
uint32_t sample_noflash(void) {
  uint32_t light_data;
  sensor_power(POWER_ON);
  //  HAL_ADC_Init(&hadc1);
  // Calibrate the A2D
  //   while (HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED) != HAL_OK); // must be done on each init of the converter. 
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_IC_Init(&htim2);
  //  HAL_Delay(5);
  light_data = tsl237_readsensor();
  //  HAL_ADC_DeInit(&hadc1);
  HAL_TIM_Base_DeInit(&htim2);
  HAL_TIM_IC_DeInit(&htim2);
  sensor_power(POWER_OFF);
  return (light_data);
}

  
