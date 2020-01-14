/* battery.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 06. 4.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <string.h>
#include <stdio.h>
#include "battery.h"

extern ADC_HandleTypeDef hadc1;

void batt_command(char *arguments) {
  uint32_t battery_voltage;
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    battery_voltage = read_vrefint();
    //    printf("%d.%03d\n\r",(int)battery_voltage/1000,(int)battery_voltage%1000-100 );
    printf("%d.%03d\n\r",(int)battery_voltage/1000,(int)battery_voltage%1000);
    printf("OK\n\r");
  }
}

uint32_t read_vrefint(void) {
  static uint32_t first_time_up = 1;
  static ADC_ChannelConfTypeDef sConfig = {0};
  uint32_t rawVintref;

  if (first_time_up) {
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    first_time_up = 0;
  }
  sConfig.Rank = ADC_REGULAR_RANK_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
      Error_Handler();
  }
  HAL_ADC_Start(&hadc1); 
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
  rawVintref = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
      Error_Handler();
  }
  return(__HAL_ADC_CALC_VREFANALOG_VOLTAGE(rawVintref, ADC_RESOLUTION_12B)-100);
}
