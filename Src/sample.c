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
#include <stdlib.h>
#include "flash.h"
#include "rtc.h"
#include "battery.h"
#include "temperature.h"
#include "tsl237.h"
#include "sample.h"
#include "power.h"
#include "cal.h"

extern flash_status_t fs;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern RTC_HandleTypeDef hrtc;

extern RTC_AlarmTypeDef sAlarm;

void sample_command(char * arguments) {
  if (arguments) {
    printf("NOK\n\r");
    return;
  }
  sample();
  printf("OK\n\r");
}
  
void sample_frequency_command(char * arguments) {
  char * argument;
  int argument_count = 0;
  int minutes;
  int hours;
  if (!arguments) {
    printf("NOK\n\r");
    return;
  }
  argument = strtok(arguments,",");
  if (!argument) {
    printf("NOK\n\r");
    return;
  }
  while (argument) {
    //    printf("%d: %s\n\r",argument_count,argument);
    switch (argument_count) {
    case 0:
      minutes = (int) strtol(argument,NULL,10);
      hours = (int) minutes/60;
      minutes = (int) minutes%60;
      printf("hours = %d minutes = %d\n\r",hours, minutes);
      break;
    default:
      break;
    }
    argument_count++;
    argument=strtok(NULL,",");
  }
  if (argument_count>1) {
    printf("NOK\n\r");
  }
  else {

    //    sAlarm.AlarmTime.Minutes = (current_time.Minutes + SAMPLE_INTERVAL_MINUTES) % 60; // Set the alarm 5 minutes in the future
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask =  RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_SECONDS; // Only consider the minutes value
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 0x1;
    sAlarm.Alarm = RTC_ALARM_A;
    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK) {
      Error_Handler();
    }
    printf("OK\n\r");
  }
}

void sample(void) {
  uint32_t raw_sensor_counts;
  uint32_t compensated_sensor_counts;
  uint32_t temperature;
  sensor_power(POWER_ON);
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_IC_Init(&htim2);
  HAL_Delay(3);
  raw_sensor_counts = tsl237_readsensor();

  HAL_ADC_Init(&hadc1);
  // Calibrate the A2D
  while (HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED) != HAL_OK); // must be done on each init of the converter. 
  //  write_sensor_data(&fs,read_vrefint(),read_temp(),tsl237_readsensor());
  //  printf("battery value=%d\n\r",(int)((read_battery()*488*2)/1000)); 
  //write_sensor_data(&fs,read_battery(),read_temp(),tsl237_readsensor());
  temperature = read_temp();
  compensated_sensor_counts = cal_sample_temperature_compensation(raw_sensor_counts,temperature);
  write_sensor_data(&fs,read_battery(),temperature,compensated_sensor_counts);
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

  
