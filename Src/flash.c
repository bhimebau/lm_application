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
#include "flash.h"

//extern int _etext;
extern int _edata;
extern int _sdata;
extern int __fini_array_end;

extern int flash_data_storage;


int flash_write_init(flash_status_t * fs) {
  uint64_t *p = find_sentinel();
  sensordata_t *sd = 0;
  uint16_t record_counter = 0;
  raw_t sentinel = {SENTINEL_MARK,0};
  if (p) {
    // Previously Inialized Data Storage
    p+=2;  // Point at first data location. 
    sd = (sensordata_t *) p;
    while (sd->watermark!=0xFF) {
      record_counter++;
      sd++;
    }
    fs->next_address = (uint64_t *) sd;
    fs->next_record_number = record_counter;
    return(0);
  }
  else {
    // The data storage area is uninitialized. 
    //   p = find_program_end(); // get a 16-byte aligned pointed to open flash
    p = (uint64_t *)&__fini_array_end + ((uint64_t *)&_edata - (uint64_t *)&_sdata); // Compute last address of the flashed program. 
    p+=2;   // Increment pointer by 16. 
    p = (uint64_t *) ((uintptr_t) p & ~(uintptr_t)0xF); // Align pointer on a 16 byte boundary
    if (p) {
      // write the sentinel mark to denote the start of the data area 
      fs->next_address = p;
      fs->next_record_number = 0;
      if (write_record(fs,(void *) &sentinel)) {
        return (-1);
      }
      return (0);
    }
    else {
      // Flash data area needs to be erased. Cannot write any data
      return (-1);
    }
  }
}

uint64_t * find_sentinel(void) {
  //  uint64_t *p = (uint64_t *)&flash_data_storage + ((uint64_t *)&_edata - (uint64_t *)&_sdata); // Linker variable indicating the end of the program loaded in flash.
  uint64_t *p = (uint64_t *)&__fini_array_end + ((uint64_t *)&_edata - (uint64_t *)&_sdata); // Compute last address of the flashed program. 
  p+=2;   // Increment pointer by 16. 
  p = (uint64_t *) ((uintptr_t) p & ~(uintptr_t)0xF); // Align pointer on a 16 byte boundary
                    //    void *ptr2 = (void *) ((uintptr_t) ptr1 & ~(uintptr_t) 0xfff)
  while (p<=((uint64_t*)FLASH_END)) {
    if (*p==SENTINEL_MARK) {
      return(p);                 // Return the address of the sentinel
    }
    p++;
  }
  return (0);                    // If the sentinel was note located, return a null pointer
} 

uint64_t * find_program_end(void) {
  uint64_t *p = (uint64_t *) FLASH_END;
  while (*p==0xFFFFFFFFFFFFFFFF) {
    p--;
  }
  p++;
  if (p>=((uint64_t *) FLASH_END)) {
    return(0);
  }


  if (((int) p) & ((int) 0x0F)) {
    // Make sure that the address is aligned on a 16 byte boundary. 
    p++;
  }
  return (p);
} 

/* void test_flash(void) { */
/*   uint64_t * p; */
/*   HAL_StatusTypeDef status = 0; */
/*   printf("The size of the record is %d bytes\n\r",sizeof(record_t)); */
/*   p = find_program_end(); */
/*   if (p) { */
/*     printf("Pre-Write: %p=0x%08x\n\r",p,(int)*p); */
/*     HAL_FLASH_Unlock(); */
/*     status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) p, (long long) SENTINEL_MARK); */
/*     HAL_FLASH_Lock(); */
/*     printf("Post-Write: status=%d,%p=0x%08x\n\r",status,p,(int)*p); */
/*   } */
/*   else { */
/*     printf("Space not available in the flash\n\r"); */
/*   } */
/* } */

int write_record(flash_status_t * fs, void * record) {
  raw_t * write_data;
  HAL_StatusTypeDef status = 0;

  write_data = (raw_t *) record;
  
  HAL_FLASH_Unlock();
  if ((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) fs->next_address++, write_data->data0))) {
    // Error writing flash
    HAL_FLASH_Lock();
    return (-1);
  }
  if ((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) fs->next_address++, write_data->data1))) {
    // Error writing flash
    HAL_FLASH_Lock();
    return (-1);
  }
  HAL_FLASH_Lock();
  fs->next_record_number++;
  return (0);
}


  
