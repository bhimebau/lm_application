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
#include <math.h> 
#include "flash.h"
#include "rtc.h"
#include "battery.h"
#include "temperature.h"
#include "tsl237.h"
#include "sample.h"
#include "cal.h"

int32_t calibration_ram[(DARK_MAG-BRIGHT_MAG)*10+1];

void cal_command(char *arguments) {
  if (!arguments) {
    // Show the calibration
    return;
  }
  else if (!strcmp(arguments,"blank")) {
    cal_blank();
  }
  else if (!strcmp(arguments,"sram")) {
    cal_show_sram();
  }
  else {
    printf("argument = %s\n\r",arguments);
    printf("OK\n\r");
  }
}

int cal_blank(void) {
  int i;
  for (i=0;i<CAL_MAX_INDEX;i++) {
    calibration_ram[i] = 0xFFFFFFFF;
  }
  return(0);
}

int cal_show_sram(void) {
  int i;
  for (i=0;i<CAL_MAX_INDEX;i++) {
    printf("%d.%d:%d\n\r",(i/10)+BRIGHT_MAG,i%10,(int)calibration_ram[i]);
  }
  printf("OK\n\r");
  return(0);
  
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





