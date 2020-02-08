/* tls25911.c
 * Description: Functions used to communicate with the TLS25911 light sensor. 
 *
 * Author: Bryce Himebaugh  * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rtc.h"

extern RTC_HandleTypeDef hrtc;

void ds_command(char *arguments) {
  RTC_DateTypeDef current_date = {0};
  char * argument;
  int argument_count = 0;
 
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
    switch (argument_count) {
    case 0:
      current_date.Month = (int) strtol(argument,NULL,10);
      if ((current_date.Month<1)||(current_date.Month>12)) {
        printf("NOK\n\r");
        return;
      }
      break;
    case 1:
      current_date.Date = (int) strtol(argument,NULL,10);
      if ((current_date.Date<1)||(current_date.Date>31)) {
        printf("NOK\n\r");
        return;
      }
      break;
    case 2:
      current_date.Year = (int) strtol(argument,NULL,10);
      if ((current_date.Year<0)||(current_date.Year>99)) {
        printf("NOK\n\r");
        return;
      }
      break;
    default:
      printf("NOK\n\r");
      break;
    }
    argument_count++;
    argument=strtok(NULL,",");
    
  }
  if (argument_count != 3) {
    printf("NOK\n\r");
    return;
  }
  if (HAL_RTC_SetDate(&hrtc, &current_date, RTC_FORMAT_BIN) != HAL_OK) {
    printf("NOK\n\r");
    return;
  }
  printf("OK\n\r");
}

void ts_command(char *arguments) {
  RTC_TimeTypeDef current_time;
  char * argument;
  int argument_count = 0;
  
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
      current_time.Hours = (int) strtol(argument,NULL,10);
      if ((current_time.Hours<0)||(current_time.Hours>23)) {
        printf("NOK\n\r");
        return;
      }
      break;
    case 1:
      current_time.Minutes = (int) strtol(argument,NULL,10);
      if ((current_time.Minutes<0)||(current_time.Minutes>59)) {
        printf("NOK\n\r");
        return;
      }
      break;
    case 2:
      current_time.Seconds = (int) strtol(argument,NULL,10);
      if ((current_time.Seconds<0)||(current_time.Seconds>59)) {
        printf("NOK\n\r");
        return;
      }
      break;
    default:
      printf("NOK\n\r");
      break;
    }
    argument_count++;
    argument=strtok(NULL,",");
    
  }
  if (argument_count != 3) {
    printf("NOK\n\r");
    return;
  }
  current_time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  current_time.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &current_time, RTC_FORMAT_BIN) != HAL_OK) {
    printf("NOK\n\r");
    return;
  }
  /* sAlarm.AlarmTime.Minutes = (current_time.Minutes + SAMPLE_INTERVAL_MINUTES) % 60; // Set the alarm 5 minutes in the future  */
  /* sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE; */
  /* sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET; */
  /* sAlarm.AlarmMask =  RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_SECONDS; // Only consider the minutes value  */
  /* sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL; */
  /* sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE; */
  /* sAlarm.AlarmDateWeekDay = 0x1; */
  /* sAlarm.Alarm = RTC_ALARM_A; */
  /* if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK) { */
  /*   Error_Handler(); */
  /* } */
  printf("OK\n\r");
}



void tr_command(char *arguments) {
  RTC_TimeTypeDef current_time;
  RTC_DateTypeDef current_date;
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    HAL_RTC_GetTime(&hrtc,&current_time,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc,&current_date,RTC_FORMAT_BIN);
    printf("tr,%02d,%02d,%02d\n\r",current_time.Hours,current_time.Minutes, current_time.Seconds);
    printf("OK\n\r");
  }
}

void dr_command(char *arguments) {
  RTC_TimeTypeDef current_time;
  RTC_DateTypeDef current_date;
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    HAL_RTC_GetTime(&hrtc,&current_time,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc,&current_date,RTC_FORMAT_BIN);
    printf("dr,%02d,%02d,20%02d\n\r",current_date.Month,current_date.Date,current_date.Year);
    printf("OK\n\r");
  }
}



uint32_t pack_time(RTC_TimeTypeDef *time, RTC_DateTypeDef *date) {
  uint32_t out = 0;
  uint32_t temp = 0;
  temp = (((uint32_t) date->Date) & 0b00011111) << 27;           // Add the date
  out = temp;
  //   printf("temp=0x%08x,date=%d\n\r",(unsigned int) temp, date->Date);
  temp = (((uint32_t) date->Month) & 0b00001111) << 22;          // Add the month   
  out |= temp;
  //   printf("temp=0x%08x,date=%d\n\r",(unsigned int) temp, date->Month);
  temp = ((((uint32_t) date->Year) - 19) & 0b00000111) << 19;  // Add the year 
  out |= temp;
  //   printf("temp=0x%08x,date=%d\n\r",(unsigned int) temp, date->Year);
  temp = (((uint32_t) time->Hours) & 0b00011111) << 13;           // Add the hour 
  out |= temp;
  //   printf("temp=0x%08x,hours=%d\n\r",(unsigned int) temp, time->Hours);
  temp = (((uint32_t) time->Minutes) & 0b00111111) << 6;          // Add the minute
  out |= temp;
  //   printf("temp=0x%08x,minutes=%d\n\r",(unsigned int) temp, time->Minutes);
  temp = (((uint32_t) time->Seconds) & 0b00111111);               // Add the second 
  out |= temp;
  //  printf("temp=0x%08x,seconds=%d\n\r",(unsigned int) temp, time->Seconds);
  //   printf("Done Packing Time/Date\n\r");
  return out;
}

void  unpack_time(uint32_t timeval, RTC_TimeTypeDef *time, RTC_DateTypeDef *date) {
  uint32_t temp = timeval;

  // Seconds
  temp &= 0x3F;
  time->Seconds = (uint8_t) temp;

  // Minutes 
  temp = timeval;
  temp >>= 6;
  temp &= 0x3F;
  time->Minutes = (uint8_t) temp;

  // Hours
  temp = timeval;
  temp >>= 13;
  temp &= 0x1F;
  time->Hours = (uint8_t) temp;

  // Year
  temp = timeval;
  temp >>= 19;
  temp &= 0x07;
  date->Year = (uint8_t) temp + 19;

  // Year
  temp = timeval;
  temp >>= 22;
  temp &= 0x0F;
  date->Month = (uint8_t) temp;
 
  // Day
  temp = timeval;
  temp >>= 27;
  temp &= 0x1F;
  date->Date = (uint8_t) temp;
}




