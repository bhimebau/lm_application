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

int32_t calibration_ram[CAL_MAX_INDEX];

void cal_command(char *arguments) {
  if (!arguments) {
    cal_show_sram();
    return;
  }
  else if (!strcmp(arguments,"blank")) {
    cal_blank();
  }
  else if (!strcmp(arguments,"load")) {
    if (!cal_load()) {
      printf("OK\n\r");
    }
    else {
      printf("NOK\n\r");
    }
  }
  else if (!strcmp(arguments,"store")) {
    printf("store the cal in flash - unimplemented\n\r");
  }
  else if (!strcmp(arguments,"write")) {
    printf("write a cal value to sram\n\r");
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
  int i = 0;
  printf("%2d.0:%11d ",i+BRIGHT_MAG,(int)calibration_ram[i]);
  for (i=1;i<CAL_MAX_INDEX;i++) {
    if (!(i%10)) {
      printf("\n\r%2d.0:",(i/10)+BRIGHT_MAG);
    }
    printf("%11d ",(int)calibration_ram[i]);
  }
  printf("\n\rOK\n\r");
  return(0);
}

int cal_load(void) {
  int i;
  uint32_t *p = (uint32_t *) CAL_START;
  for (i=0;i<CAL_MAX_INDEX;i++) {
    calibration_ram[i] = *p;
    p++;
  }
  return (0);
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





