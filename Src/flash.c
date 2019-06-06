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

void erase_command(char *arguments) {
  if (!arguments) {
    printf("NOK\n\r");
    return;
  }
  if (!strcmp("all",arguments)) {
    if (flash_reset(&fs)) {
      printf("NOK\n\r");
      return;
    }
  }
  printf("OK\n\r");
}


int flash_write_init(flash_status_t * fs) {
  sensordata_t *sd = 0;
  uint16_t record_counter = 0;
  raw_t sentinel_top = {SENTINEL_MARK_TOP,0};
  raw_t sentinel_bottom = {SENTINEL_MARK_BOTTOM,0};

  // Address of the end of the program
  uint64_t *program_end = (uint64_t *) ((uint32_t )&__fini_array_end 
                                        + (uint32_t)&_edata
                                        - (uint32_t)&_sdata); 
    
  // Address of the top of flash, fixed based on microcontroller variant
  uint64_t *top = (uint64_t *) FLASH_END;
  
  // Address of the bottom of flash available for records
  uint64_t *bottom = (uint64_t *) (((uint32_t)program_end & ~0x7FF) + 0x800);   
  
  // Top Memory Pointer: Located at the very top of flash
  uint64_t *pt = find_sentinel_top();
 
  // Bottom Memory Pointer: Located at the first open page after the software.
  uint64_t *pb = find_sentinel_bottom();

  /* There are 4 cases for the top and bottom memory pointers.
     1.) pt = 0, pb = 0: Memory Uninitialized
     Action: Erase memory if needed, add top and bottom sentinels
     2.) pt > 0, pb = 0: Lower sentinel has been corrupted, 
     Action: restore lower sentinel. Accept that data may have been lost 
     3.) pt = 0, pb > 0: Upper sentinel has been corrupted
     Action: Erase memory, add top and bottom sentinels
     4.) pt > 0, pb > 0: Memory is initialized
     Action: No memory changes are necessary
  */

  /* printf("top = %p\n\r",top); */
  /* printf("bottom = %p\n\r",bottom); */
  /* printf("program_end = %p\n\r",program_end); */
  /* printf("pt = %p\n\r",pt); */
  /* printf("pb = %p\n\r",pb); */
  if ((!pt) && (!pb)) {
    /* printf("Memory Uninitialized\n\r"); */
    /* printf("Writing sentinels..."); */
    if (write_sentinel(top,&sentinel_top)) {
      /* printf("Could not write top sentinel\n\r"); */
      return(-1);
    } 
    if (write_sentinel(bottom,&sentinel_bottom)) {
      /* printf("Could not write top sentinel\n\r"); */
      return(-1);
    }
    /* printf("Done\n\r"); */
  }
  else if ((pt) && (!pb)) {
    /* printf("Lower Sentinel has been corrupted, likely from program download\n\r"); */
    /* printf("Writing lower sentinel..."); */
    if (write_sentinel(bottom,&sentinel_bottom)) {
      /* printf("Could not write top sentinel\n\r"); */
      return(-1);
    }
    /* printf("Done\n\r"); */
  }
  else if ((!pt) && (pb)) {
    /* printf("Memory corrupted, Erase not implemented for this stage.\n\r"); */
    return (-1);
  }
  else if ((pt) && (pb)) {
    /* printf("Memory initialized and ready to go.\n\r"); */
    if (pb!=bottom) {
      /* printf("Sentinel is not at the bottom of the lowest flash page\n\r"); */
      return (-1);
    }
  }
  else {
    /* printf("Unknown memory condition\n\r"); */
    return (-1);
  }
  //If the program make it this far, both sentinels are in place at top and bottom, respectively
  //The next step is to load the flash data structure used for writing/reading records
  fs->data_start = top - 2; // "top" is the sentinel, 1 below it is the first record address
  fs->max_possible_records = (((uint32_t)top-(uint32_t)bottom)>>4)-1;
  sd = (sensordata_t *) top; // sd is not pointing at the top sentinel.
  sd--; // Pointing at the first data record
  while ((sd->watermark!=0xFF)&&(sd->watermark!=0xa5)) {
    record_counter++;
    sd--;
  }
  fs->next_record_number = record_counter;
  fs->total_records = record_counter;
  fs->next_address = (uint64_t *) sd;  
  /* printf("data_start=%p\n\r",fs->data_start); */
  /* printf("max_possible_records=%d\n\r",(int) fs->max_possible_records); */
  /* printf("next_record_number=%d\n\r",(int) fs->next_record_number); */
  /* printf("total_records=%d\n\r",(int) fs->total_records); */
  /* printf("next_adddress=%p\n\r",fs->next_address); */
  return (0);
}
  
static uint32_t GetPage(uint32_t Addr) {
  uint32_t page = 0;
  
  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }
  
  return page;
}

static uint32_t GetBank(uint32_t Addr) {
  return FLASH_BANK_1;
}

int flash_erase(void) {
  uint32_t FirstPage = 0, NbOfPages = 0, BankNumber = 0;
  uint32_t PAGEError = 0;
  /* __IO uint32_t data32 = 0 , MemoryProgramStatus = 0; */

  static FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t program_end = ((uint32_t )&__fini_array_end 
                  + (uint32_t)&_edata
                  - (uint32_t)&_sdata); 
  uint32_t bottom  = (((uint32_t)program_end & ~0x7FF) + 0x800);
  uint32_t top = FLASH_END;
  
  HAL_FLASH_Unlock();
  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
  /* Get the 1st page to erase */
  FirstPage = GetPage(bottom);
  /* Get the number of pages to erase from 1st page */
  NbOfPages = GetPage(top) - FirstPage + 1;
  /* Get the bank */
  BankNumber = GetBank(bottom);
  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = BankNumber;
  EraseInitStruct.Page        = FirstPage;
  EraseInitStruct.NbPages     = NbOfPages;

  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
     you have to make sure that these data are rewritten before they are accessed during code
     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
     DCRST and ICRST bits in the FLASH_CR register. */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
    HAL_FLASH_Lock();
    return (-1);
  }
  HAL_FLASH_Lock();
  return(0);
} 

int flash_reset(flash_status_t * fs) {
  if (flash_erase()) {
    return (-1);
  }
  if (flash_write_init(fs)) {
    return (-1);
  }
  return(0);
}

/*   // Data that will be written if the memory has not been initialized yet. */
     
/*   if (pt) { */
/*     // Previously Inialized Data Storage, Sentinel located. */
/*     pt-=2;               // Point at first data location. */
/*     fs->data_start = p;  // Start of memory will be at the top of flash */
/*     if (pb) { */
/*       // bottom sentinel location was found. */
/*       fs->max_possible_records = (pt-pb-(uint64_t *)2); // Compute the total slots */
/*     } */
/*     else { */
/*       // the bottom sentinel mark is missing even though the top sentinel is found */
/*       // This can occur when new code is loaded and the flash page gets erased. */
/*       // A new bottom sentinel needs to be added. */

/*       // Compute last address of the flashed program. */
/*       pb = (uint64_t *)&__fini_array_end */
/*         + ((uint64_t *)&_edata */
/*            - (uint64_t *)&_sdata); */

/*       // Compute the first address of the next flash page */
/*       // This is necessary because subsequent code downloads will erase up to the end of the page. */
/*       // This will reduce the number of times the bottom sentinel will get erased. */
/*       // However, this will not stop the sentinel from getting erased by a large code change. */
/*       pb = (pb & ~((uint64_t *) 0x7FF)) + 1; */
      
/*       pb+=2;   // Increment pointer by 16. */
/*       pb = (uint64_t *) ((uintptr_t) p & ~(uintptr_t)0xF); // Align pointer on a 16 byte boundary */


      
/*       // write the sentinel mark to denote the start of the data area */
/*       fs->next_address = p; */
/*       if (write_record(fs,(void *) &sentinel)) { */
/*         return (-1); */
/*       } */
/*       fs->next_record_number = 0; // Set the first record number to 0, the sentinel does not count. */
/*       p+=2; */
/*       fs->data_start = p;  // The data will start at one location beyond the sentinel. */
/*       fs->max_possible_records = ((uint32_t)0x0803FFF0 - (uint32_t) p) >> 4; */
/*       fs->total_records = 0; */
/*       return (0); */
/*     } */
/*     fs->max_possible_records = ((uint32_t)0x0803FFF0 - (uint32_t) p) >> 4; */
/*     sd = (sensordata_t *) p; */
/*     while (sd->watermark!=0xFF) { */
/*       record_counter++; */
/*       sd++; */
/*     } */
/*     fs->next_address = (uint64_t *) sd; */
/*     fs->next_record_number = record_counter; */
/*     fs->total_records = record_counter; */
/*     return(0); */
/*   } */
/*   else { */
/*     // The data storage area is uninitialized. Write the sentinel to the first data slot. */
/*     p = (uint64_t *)&__fini_array_end + ((uint64_t *)&_edata - (uint64_t *)&_sdata); // Compute last address of the flashed program. */
/*     p+=2;   // Increment pointer by 16. */
/*     p = (uint64_t *) ((uintptr_t) p & ~(uintptr_t)0xF); // Align pointer on a 16 byte boundary */
/*     if (p) { */
/*       // write the sentinel mark to denote the start of the data area */
/*       fs->next_address = p; */
/*       if (write_record(fs,(void *) &sentinel)) { */
/*         return (-1); */
/*       } */
/*       fs->next_record_number = 0; // Set the first record number to 0, the sentinel does not count. */
/*       p+=2; */
/*       fs->data_start = p;  // The data will start at one location beyond the sentinel. */
/*       fs->max_possible_records = ((uint32_t)0x0803FFF0 - (uint32_t) p) >> 4; */
/*       fs->total_records = 0; */
/*       return (0); */
/*     } */
/*     else { */
/*       // Flash data area needs to be erased. Cannot write any data */
/*       return (-1); */
/*     } */
/*   } */
/* } */

uint64_t * find_sentinel_bottom(void) {
  uint64_t *p = (uint64_t *)&__fini_array_end + ((uint64_t *)&_edata - (uint64_t *)&_sdata); // Compute last address of the flashed program. 
  p+=2;   // Increment pointer by 16. 
  p = (uint64_t *) ((uintptr_t) p & ~(uintptr_t)0xF); // Align pointer on a 16 byte boundary
  while (p<=((uint64_t*)FLASH_END)) {
    if (*p==SENTINEL_MARK_BOTTOM) {
      return(p);                 // Return the address of the sentinel
    }
    p+=2;                        // Move pointer by 16 bytes on each iteration
  }
  return (0);                    // If the sentinel was not located, return a null pointer
} 

uint64_t * find_sentinel_top(void) {
  uint64_t *p = (uint64_t *) FLASH_END;
  if (*p==SENTINEL_MARK_TOP) {
    return(p);                 // Return the address of the sentinel
  }
  else {
    return (0);                // If the sentinel was note located, return a null pointer
  }
} 

int write_sentinel(uint64_t * location, raw_t * sentinel) {
  if ((!location) || (!sentinel)) {
    return(-1);
  }
  HAL_FLASH_Unlock();
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) location++, sentinel->data0)) {
    HAL_FLASH_Lock();
    return (-1);
  }
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) location, sentinel->data1)) {
    HAL_FLASH_Lock();
    return (-1);
  }
  HAL_FLASH_Lock();
  return (0);
}

int write_record(flash_status_t * fs, void * record) {
  raw_t * write_data;
  HAL_StatusTypeDef status = 0;

  if ((!fs) || (!record)) {
    return (-1);
  }

  if (fs->total_records >= fs->max_possible_records) {
    printf("flash full\n\r");
    return (-1);
  }
   
  write_data = (raw_t *) record;
  
  HAL_FLASH_Unlock();
  if ((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) fs->next_address++, write_data->data0))) {
    HAL_FLASH_Lock();
    return (-1);
  }
  if ((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) fs->next_address, write_data->data1))) {
    HAL_FLASH_Lock();
    return (-1);
  }
  HAL_FLASH_Lock();
  fs->next_address-=3;
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
    while ((p->watermark!=0xFF)&&(p->watermark!=0xa5)) {
      unpack_time(p->timestamp,&time,&date);
      switch (p->status) {
      case DATA_RECORD:
        if ((type == DATA_RECORD) || (type == ALL_RECORD)) {
          printf("D,");
          printf("%d,",p->record_number);
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
          printf("%d,",p->record_number);
          printf("%02d/%02d/20%02d,",date.Month,date.Date,date.Year);
          printf("%02d:%02d:%02d,",time.Hours,time.Minutes,time.Seconds);
          printf("%s\n\r",((logdata_t *)p)->msg);
        }
        break;
      default:
        printf("NOK\n\r");
      }
      p--;
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
  if (write_record(fs,&p)) {
    return (-1); // Write failed
  }
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
  if (write_record(fs,&p)) {
    return (-1); // write failed
  }
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

