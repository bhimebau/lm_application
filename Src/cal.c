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
  long value;
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
  else if (!strcmp(argv[0],"fillup")) {
    cal_fill_test_ascending();
  }
  else if (!strcmp(argv[0],"filldown")) {
    cal_fill_test_descending();
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
  else if (!strcmp(argv[0],"lookup")) {
    value = strtol(argv[1],NULL,10);
    if ((value=cal_lookup(value))!=-1) {
      printf("%d.%d\n\r",(int)(value/100),(int)(value%100));
      printf("OK\n\r");
    }
    else {
      printf("NOK\n\r");
    }
  }
  else if (!strcmp(argv[0],"complete")) {
    cal_complete();
    printf("OK\n\r");
  }
  else if (!strcmp(argv[0],"fake")) {
    cal_fake();
    printf("OK\n\r");
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

int cal_fill_test_ascending(void) {
  int i;
  int data = 100;
  for (i=0;i<CAL_MAX_INDEX;i++) {
    calibration_ram[i] = data;
    data+=100;
  }
  return(0);
}

int cal_fill_test_descending(void) {
  int i;
  int data = 20100;
  for (i=0;i<CAL_MAX_INDEX;i++) {
    calibration_ram[i] = data;
    data-=100;
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

int cal_lookup(uint32_t count) {
  int i;
  int index = -1;
  int delta = 0;
  for (i=0;i<(CAL_MAX_INDEX-1);i++) {
    // Acending Values 
    if (calibration_ram[i] < calibration_ram[i+1]) {
      if ((count >= calibration_ram[i]) &&
          (count <= calibration_ram[i+1])) {
        index = i;
        delta = (calibration_ram[i+1]-calibration_ram[i])/10;
        count = (count-calibration_ram[i])/delta;
        index = (index + (BRIGHT_MAG*10))*10;
        index = index + count;
        break;
      }  
    }
    // Decending Values
    else if (calibration_ram[i] > calibration_ram[i+1]) {
      if ((count <= calibration_ram[i]) &&
          (count >= calibration_ram[i+1])) {
        index = i;
        delta = (calibration_ram[i]-calibration_ram[i+1])/10;
        count = (count-calibration_ram[i+1])/delta;
        if (count == 0) {
          index = index+1;
        }
        index = (index + (BRIGHT_MAG*10))*10;
        index = index + count;
        break;
      }  
    }
    // Flat Values
    else {
      if (count==calibration_ram[i]) {
        index = i;
        count = 0;
        break;
      }
    }
  }
  return(index);
}

int cal_complete(void) {
  int i;
  float x1,x2,y1,y2;
  float m, b;
  float value;
  int begin = 0;
  int end = 0;
  int found = 0;
  int last_pass = 0;
    // see of the first element in the table has a value.
  begin = 0;
  while (!last_pass) {
    if (calibration_ram[begin]==-1) {
      x1 = begin;
      y1 = CAL_MIN_VALUE;
      calibration_ram[begin] = CAL_MIN_VALUE;
    }
    else {
      x1 = begin;
      y1 = calibration_ram[begin];
    }
    found = 0;
    for (i=begin+1;i<CAL_MAX_INDEX;i++) {
      if (calibration_ram[i] != -1) {
        end = i;
        x2=end;
        y2=calibration_ram[i];
        found = 1;
        break;
      }
    }
    if (!found) {
      i = CAL_MAX_INDEX;
      x2 = CAL_MAX_INDEX;
      y2 = CAL_MAX_VALUE;
      end = CAL_MAX_INDEX-1;
      calibration_ram[end] = CAL_MAX_VALUE;
    }
    if (i == CAL_MAX_INDEX) {
      last_pass = 1;
    }
    /* printf("begin=%d, end=%d\n\r",begin, end); */
    /* printf("x1=%f,y1=%f  x2=%f,y2=%f\n\r",x1,y1,x2,y2); */
    // compute the slope
    m = (y2-y1)/(x2-x1);
    
    // compute the y-intercept
    b = y1 - (m * x1);
    
    // print the equation for the line 
    /* printf("line equation: y = %.2f*x + %.2f\n\r",m,b); */
    
    // Compute the intermediate points
    for (i=begin+1;i<end;i++) {
      value = m * (float) i + b;
      calibration_ram[i] = (int) value;
    }
    begin = end;
  }
  return (0);
}

int cal_fake(void) {
  cal_blank();
  calibration_ram[52] = 1000;
  calibration_ram[107] = 3000;
  calibration_ram[142] = 6000;
  calibration_ram[195] = 13000;
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





