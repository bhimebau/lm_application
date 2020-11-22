/* temperature.c
 * Description: STM32L432KC Temperture Read Function 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 04.20.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <string.h>
#include <stdio.h>
#include "temperature.h"
#include "battery.h"

extern ADC_HandleTypeDef hadc1;


void temp_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    HAL_ADC_Init(&hadc1);
    while (HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED) != HAL_OK);
    printf("%d\n\r",(int)read_temp());
    HAL_ADC_DeInit(&hadc1);
    printf("OK\n\r");
  }
}

int32_t read_temp(void) {
  static uint32_t first_time_up = 1;
  static ADC_ChannelConfTypeDef sConfig = {0};
  uint32_t rawTemp;
  uint32_t vref;
  
    
  if (first_time_up) {
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    //sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
    sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
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
  rawTemp = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  /* sConfig.Rank = ADC_REGULAR_RANK_2; */
  /* if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { */
  /*     Error_Handler(); */
  /* } */
  vref = read_vrefint();
  return (__HAL_ADC_CALC_TEMPERATURE(vref,
                                     rawTemp,
                                     ADC_RESOLUTION_12B));
}


  
/* uint32_t read_vrefint(void) { */
/*   static uint32_t first_time_up = 1; */
/*   static ADC_ChannelConfTypeDef sConfig = {0}; */
/*   uint32_t rawVintref; */

/*   if (first_time_up) { */
/*     sConfig.Channel = ADC_CHANNEL_VREFINT; */
/*     sConfig.Rank = ADC_REGULAR_RANK_1; */
/*     sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5; */
/*     sConfig.SingleDiff = ADC_SINGLE_ENDED; */
/*     sConfig.OffsetNumber = ADC_OFFSET_NONE; */
/*     sConfig.Offset = 0; */
/*     first_time_up = 0; */
/*   } */
/*   sConfig.Rank = ADC_REGULAR_RANK_1; */
/*   if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { */
/*       Error_Handler(); */
/*   } */
/*   HAL_ADC_Start(&hadc1);  */
/*   HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); */
/*   rawVintref = HAL_ADC_GetValue(&hadc1); */
/*   HAL_ADC_Stop(&hadc1); */
/*   sConfig.Rank = ADC_REGULAR_RANK_2; */
/*   if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { */
/*       Error_Handler(); */
/*   } */
/*   return(__HAL_ADC_CALC_VREFANALOG_VOLTAGE(rawVintref, ADC_RESOLUTION_12B)); */
/* } */


