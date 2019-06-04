/* flash.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 04. 7.2019
 * Copyright (C) 2019
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

extern int _edata;
extern int _sdata;
extern int __fini_array_end;
extern RTC_HandleTypeDef hrtc;
extern flash_status_t fs;

void data_command(char * arguments) {
  if (arguments) {
    printf("NOK\n\r");
    return;
  }
  read_all_records(&fs,DATA_RECORD);
}

void log_command(char * arguments) {
  if (arguments) {
    printf("NOK\n\r");
    return;
  }
  read_all_records(&fs,LOG_RECORD);
}

int flash_write_init(flash_status_t * fs) {
  uint64_t *p = find_sentinel();
  sensordata_t *sd = 0;
  uint16_t record_counter = 0;
  raw_t sentinel = {SENTINEL_MARK,0};
  if (p) {
    // Previously Inialized Data Storage, Sentinel located. Point at first open data slot. 
    p+=2;  // Point at first data location.
    fs->data_start = p;
    fs->max_possible_records = ((uint32_t)0x0803FFF0 - (uint32_t) p) >> 4;
    sd = (sensordata_t *) p;
    while (sd->watermark!=0xFF) {
      record_counter++;
      sd++;
    }
    fs->next_address = (uint64_t *) sd;
    fs->next_record_number = record_counter;
    fs->total_records = record_counter;
    return(0);
  }
  else {
    // The data storage area is uninitialized. Write the sentinel to the first data slot. 
    p = (uint64_t *)&__fini_array_end + ((uint64_t *)&_edata - (uint64_t *)&_sdata); // Compute last address of the flashed program. 
    p+=2;   // Increment pointer by 16. 
    p = (uint64_t *) ((uintptr_t) p & ~(uintptr_t)0xF); // Align pointer on a 16 byte boundary
    if (p) {
      // write the sentinel mark to denote the start of the data area 
      fs->next_address = p;
      if (write_record(fs,(void *) &sentinel)) {
        return (-1);
      }
      fs->next_record_number = 0; // Set the first record number to 0, the sentinel does not count. 
      p+=2;
      fs->data_start = p;  // The data will start at one location beyond the sentinel.
      fs->max_possible_records = ((uint32_t)0x0803FFF0 - (uint32_t) p) >> 4;
      fs->total_records = 0;
      return (0);
    }
    else {
      // Flash data area needs to be erased. Cannot write any data
      return (-1);
    }
  }
}

uint64_t * find_sentinel(void) {
  uint64_t *p = (uint64_t *)&__fini_array_end + ((uint64_t *)&_edata - (uint64_t *)&_sdata); // Compute last address of the flashed program. 
  p+=2;   // Increment pointer by 16. 
  p = (uint64_t *) ((uintptr_t) p & ~(uintptr_t)0xF); // Align pointer on a 16 byte boundary
  while (p<=((uint64_t*)FLASH_END)) {
    if (*p==SENTINEL_MARK) {
      return(p);                 // Return the address of the sentinel
    }
    p++;
  }
  return (0);                    // If the sentinel was note located, return a null pointer
} 

int write_record(flash_status_t * fs, void * record) {
  raw_t * write_data;
  HAL_StatusTypeDef status = 0;

  write_data = (raw_t *) record;
  
  HAL_FLASH_Unlock();
  if ((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) fs->next_address++, write_data->data0))) {
    HAL_FLASH_Lock();
    return (-1);
  }
  if ((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) fs->next_address++, write_data->data1))) {
    HAL_FLASH_Lock();
    return (-1);
  }
  HAL_FLASH_Lock();
  fs->next_record_number++;
  fs->total_records++;
  return (0);
}

int read_all_records(flash_status_t * fs, int type) {
  sensordata_t * p = (sensordata_t *) fs->data_start;
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
    
  
  if (p->watermark == 0xFF) {
    return (0);
  }
  else {
    while (p->watermark!=0xFF) {
      unpack_time(p->timestamp,&time,&date);
      switch (p->status) {
      case DATA_RECORD:
        if ((type == DATA_RECORD) || (type == ALL_RECORD)) {
          printf("D,");
          printf("%02d/%02d/20%02d,",date.Month,date.Date,date.Year);
          printf("%02d:%02d:%02d,",time.Hours,time.Minutes,time.Seconds);
          printf("%d.%03d,",(int) p->battery_voltage/1000,(int) p->battery_voltage%1000);
          printf("%d,",p->temperature);
          printf("%f\n\r",p->lux);
        }
        break;
      case LOG_RECORD:
        if ((type == LOG_RECORD) || (type == ALL_RECORD)) {          
          printf("L,");
          printf("%02d/%02d/20%02d,",date.Month,date.Date,date.Year);
          printf("%02d:%02d:%02d,",time.Hours,time.Minutes,time.Seconds);
          printf("%s\n\r",((logdata_t *)p)->msg);
        }
        break;
      default:
        printf("NOK\n\r");
      }
      p++;
    }
    printf("OK\n\r");
  }
  return(0);
}

int write_sensor_data(flash_status_t *fs,
                      uint16_t battery_voltage,
                      uint16_t temperature,
                      float lux) {
  RTC_TimeTypeDef current_time;
  RTC_DateTypeDef current_date;
  
  
  HAL_RTC_GetTime(&hrtc,&current_time,RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc,&current_date,RTC_FORMAT_BIN);
  sensordata_t p = {0x01,
                    DATA_RECORD,
                    fs->next_record_number,
                    pack_time(&current_time,&current_date),
                    battery_voltage,
                    temperature,
                    lux
  };
  write_record(fs,&p);
  return(0);
}

int write_log_data(flash_status_t *fs,
                   char * write_string){
  
  RTC_TimeTypeDef current_time;
  RTC_DateTypeDef current_date;
   
  HAL_RTC_GetTime(&hrtc,&current_time,RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc,&current_date,RTC_FORMAT_BIN);
  
  logdata_t p = {0x01,
                 LOG_RECORD,
                 fs->next_record_number,
                 pack_time(&current_time,&current_date),
                 {0,0,0,0,0,0,0,0}
  };
  strncpy((char *) p.msg,write_string,7);  // Copy the first 7 characters of the string
                                  // NULL character is provided by the initialization of p 
  write_record(fs,&p);
  return(0);
}

int report_flash_status(flash_status_t *fs) {
  printf("Starting Location of Flash Data = %p\n\r",fs->data_start);
  printf("Total Number of Records in the Flash Now = %d\n\r",(int) fs->total_records);
  printf("Maximum possible records = %d\n\r",(int) fs->max_possible_records);
  printf("Next Address to Write = %p\n\r",fs->next_address);
  printf("Next Record Number = %d\n\r",(int) fs->next_record_number);
  return(0);
}

