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
#include "power.h"

extern ADC_HandleTypeDef hadc1;

/* void batt_command(char *arguments) { */
/*   //  uint32_t battery_voltage; */
/*   if (arguments) { */
/*     printf("NOK\n\r"); */
/*   } */
/*   else { */
/*     //    battery_voltage = read_vrefint(); */
/*     //    printf("%d.%03d\n\r",(int)battery_voltage/1000,(int)battery_voltage%1000-100 ); */
/*     //    printf("%d.%03d\n\r",(int)battery_voltage/1000,(int)battery_voltage%1000); */
/*     printf("raw voltage = 0x%0x\n\r",(int) read_battery()); */
/*     printf("OK\n\r"); */
/*   } */
/* } */

uint32_t read_battery(void) {
  static uint32_t first_time_up = 1;
  static ADC_ChannelConfTypeDef sConfig = {0};
  uint32_t vbatt_raw;
  if (first_time_up) {
    sConfig.Channel = ADC_CHANNEL_11;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    /* sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5; */
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
  vbatt_raw = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  /* sConfig.Rank = ADC_REGULAR_RANK_2; */
  /* if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { */
  /*     Error_Handler(); */
  /* } */
  /* return(__HAL_ADC_CALC_VREFANALOG_VOLTAGE(rawVintref, ADC_RESOLUTION_12B)); */
  /* Battery Voltage Computation 
     
     HAL_ADC_GetValue() returns the number of counts from a 12-bit ADC
     (4096 counts) that has a 2V reference. Each count represents a
     2V/4096 = 488uV step per count. The counts are multiplied by
     488uV to get the voltage at the ADC in uV. The battery voltage
     runs through a 1Kx1K divider before arriving at the ADC, so the
     battery voltage would be 2 times the voltage at the converter.

     At this point, the computation represents the battery voltage in
     uV. It is only necessary to have the voltage in mV. The
     additional precision is eliminated by dividing by 1000.
  */
  /* printf("Reference Voltage = %d\n\r",(int) read_vrefint()); */
  /* printf("Raw Converted Counts = %d\n\r",(int) vbatt_raw); */
  //  vbatt_raw = (vbatt_raw * 488 * 2)/1000;
  vbatt_raw = (vbatt_raw * (int) read_vrefint() * 2)/4096;
  /* printf("Computed Battery Voltage = %d\n\r",(int) vbatt_raw); */
  
  return(vbatt_raw);
}

uint32_t read_vrefint(void) {
  static uint32_t first_time_up = 1;
  static ADC_ChannelConfTypeDef sConfig = {0};
  uint32_t rawVintref;

  if (first_time_up) {
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    //    sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
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
  rawVintref = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
      Error_Handler();
  }
  return(__HAL_ADC_CALC_VREFANALOG_VOLTAGE(rawVintref, ADC_RESOLUTION_12B));
}
