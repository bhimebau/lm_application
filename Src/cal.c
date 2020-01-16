/* cal.c
 * Description: Functions to process the calibration for the light sensor. 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.15.2020
 * Copyright (C) 2020
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
#include "tsl237.h"
#include "sample.h"

void cal_command(char *arguments) {
  if (!arguments) {
    // Show the calibration
    return;
  }
  printf("argument = %s\n\r",arguments);
  printf("OK\n\r");
}


int flash_caldata(int index, caldata_t * val) {
  HAL_StatusTypeDef status;
  uint64_t *q = (uint64_t *) val;
  
  if ((index > 255) || (index < 0) || (!val)) {
    return (-1);
  }
  caldata_t *p = (caldata_t *) CAL_START + index;

  /* Check to see if the particular flash location is blank */
  if (*((int64_t *) p) != 0xFFFFFFFFFFFFFFFF) {
    return (-1);
  }
  HAL_FLASH_Unlock();
  if ((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) p, *q))) {
    HAL_FLASH_Lock();
    return (-1);
  }
  HAL_FLASH_Lock();
  return(0);
}





