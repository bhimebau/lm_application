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

extern ADC_HandleTypeDef hadc1;

/*  *         @arg @ref ADC_RESOLUTION_12B */
/*   *         @arg @ref ADC_RESOLUTION_10B */
/*   *         @arg @ref ADC_RESOLUTION_8B */
/*   *         @arg @ref ADC_RESOLUTION_6B */
/*   * @retval Temperature (unit: degree Celsius) */
/*   *\/ */
/* #define __HAL_ADC_CALC_TEMPERATURE(__VREFANALOG_VOLTAGE__,\ */
/*                                    __TEMPSENSOR_ADC_DATA__,\ */
/*                                    __ADC_RESOLUTION__)              */

int32_t read_temp(void) {
  // Grab the temperature 
  uint32_t rawValue;
  /* float temp; */

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
  rawValue = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  printf("Cal1 = 0x%08x\n\r",(*(int *)TEMPSENSOR_CAL1_ADDR));
  printf("Cal2 = 0x%08x\n\r",(*(int *)TEMPSENSOR_CAL2_ADDR));
  

  return(__HAL_ADC_CALC_TEMPERATURE(__VREFANALOG_VOLTAGE__,
                                    rawValue,
                                    ADC_RESOLUTION_12B));
   
  /* return(__HAL_ADC_CALC_TEMPERATURE((uint32_t)3750, */
  /*                                   rawValue, */
  /*                                   ADC_RESOLUTION_12B)); */
         
  /* temp = ((float)rawValue) / 4095 * 3300; */
  /* temp = ((temp - 760.0) / 2.5) + 25; */
  /* return(temp); */
__HAL_ADC_CALC_VREFANALOG_VOLTAGE
  
}


