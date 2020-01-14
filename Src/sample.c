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

extern flash_status_t fs;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

void sample(void) {
  HAL_ADC_Init(&hadc1);
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_IC_Init(&htim2);
  write_sensor_data(&fs,read_vrefint(),read_temp(),tsl237_readsensor());
  HAL_ADC_DeInit(&hadc1);
  HAL_TIM_Base_DeInit(&htim2);
  HAL_TIM_IC_DeInit(&htim2);
}

