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
#include "rtc.h"

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




