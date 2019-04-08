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
#include "flash.h"

/* find_deadbeef() { */

/* } */


uint32_t * find_flash_start(void) {
  uint32_t *p = (uint32_t *) FLASH_END;
  while (*p==0xFFFFFFFF) {
    p--;
  }
  p++;
  if (p==((uint32_t *) 0x8040000)) {
    // If the pointer is at the end of memory
    // then set the pointer to null
    // the memory is full. 
    p = 0; 
  }
  return (p);
} 

void test_flash(void) {
  uint32_t * p;
  HAL_StatusTypeDef status = 0;
  p = find_flash_start();
  if (p) {
    printf("Pre-Write: %p=0x%08x\n\r",p,(int)*p);
    HAL_FLASH_Unlock();
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) p, (long long) SENTINEL_MARK);
    HAL_FLASH_Lock();
    printf("Post-Write: status=%d,%p=0x%08x\n\r",status,p,(int)*p);
  }
  else {
    printf("Space not available in the flash\n\r");
  }
}

