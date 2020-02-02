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
#include <stdlib.h>
#include "flash.h"
#include "rtc.h"
#include "battery.h"
#include "temperature.h"
#include "tsl237.h"
#include "sample.h"
#include "cal.h"

#define MAX_ARGS 3
int32_t calibration_ram[CAL_MAX_INDEX];

void cal_command(char *arguments) {
  char * argv[MAX_ARGS];
  char *p = arguments;
  int i;
  if (!arguments) {
    cal_show_sram();
    return;
  }
  else {
    // Zero the arguments array 
    for (i=0;i<MAX_ARGS;i++) {
      argv[i] = (char *) 0;
    }
    argv[0] = p;
    i = 1;
    // locate the other arguments
    while ((*p) && (i < MAX_ARGS)){
      if (*p==',') {
        *p=0;
        argv[i++] = p+1;
      }
      p++;
    }
  }
   
  if (!strcmp(argv[0],"blank")) {
    cal_blank();
  }
  else if (!strcmp(argv[0],"load")) {
    if (!cal_f2r()) {
      printf("OK\n\r");
    }
    else {
      printf("NOK\n\r");
    }
  }
  else if (!strcmp(argv[0],"store")) {
    if (!cal_r2f()) {
      printf("OK\n\r");
    }
    else {
      printf("NOK\n\r");
    }
  }
  else if (!strcmp(argv[0],"write")) {
    if (!cal_write(argv[1],sample_noflash())) {
      printf("OK\n\r");
    }
    else {
      printf("NOK\n\r");
    }
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

int cal_f2r(void) {
  int i;
  uint32_t *p = (uint32_t *) CAL_START;
  for (i=0;i<CAL_MAX_INDEX;i++) {
    calibration_ram[i] = *p;
    p++;
  }
  return (0);
}

int cal_r2f(void) {
  int i;
  uint64_t *p = (uint64_t *) CAL_START;
  uint64_t *q = (uint64_t *) calibration_ram;
  int ret_val = 0;
  HAL_StatusTypeDef status;
  
  cal_erase();  // Erase the flash page that holds the calibration
  HAL_FLASH_Unlock();
  // Copy the calibration_ram array to flash
  for (i=0;i<((CAL_MAX_INDEX/2)+(CAL_MAX_INDEX%2));i++) {
    if ((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,(int) p, *q))) {
      ret_val = -1;
      break;
    }
    p++;  // Stepping through addresses in 8 byte chunks
    q++;  // Stepping through the data in 8 byte chunks. 
  }
  HAL_FLASH_Lock();
  return (ret_val);
}

int cal_write(char * mag, uint32_t data) {
  char * p = 0;  // Used to hold the whole number portion
  char * q = 0;   // Used to hold the decimal portion
  long index;

  // Make sure that this is a good pointer
  if (!mag) {
    return(-1);
  }

  // Confirm that mag only include digits and optionally
  // a period
  p = mag;
  while (*p) {
    if (!(((*p>='0') && (*p<='9')) ||
          (*p=='.'))) {
      return(-1);
    }
    p++;
  }
  
  // Remove the period, only 1 decimal place allowed.
  // Deal with the whole number portion first
  p = mag;
  while (*p) {
    if (*p == '.') {
      *p = 0;
      q = p+1;
      break;
    }
    p++;
  }
  // Deal with the decimal portion
  // Only allow 1 digit
  if (q) {
    if (*(q+1)!=0) {
      *(q+1) = 0;
    }
  }
   
  // Convert the number to an integer
  if (q) {
    index = ((strtol(mag,NULL,10)-BRIGHT_MAG)*10)+strtol(q,NULL,10);
  }
  else {
    index = ((strtol(mag,NULL,10)-BRIGHT_MAG)*10);
  }
  
  // Check to make sure the number is in range
  if ((index < 0) || index > (CAL_MAX_INDEX)) {
    return(-1);
  }
  else {
    calibration_ram[index] = data;
  }
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





