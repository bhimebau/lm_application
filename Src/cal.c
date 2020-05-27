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
int32_t calibration_ram[CAL_SIZE];

void cal_command(char *arguments) {
  char * argv[MAX_ARGS];
  char *p = arguments;
  int i;
  long value;
  int magarc = 0;
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
    //    if (!cal_write(argv[1],sample_noflash())) {
    if (!cal_write(argv[1],argv[2])) {
      printf("OK\n\r");
    }
    else {
      printf("NOK\n\r");
    }
  }
  else if (!strcmp(argv[0],"lookup")) {
    value = strtol(argv[1],NULL,10);
    magarc = cal_lookup(value);
    if ((magarc == 1) || (magarc == -1)) {
      printf("%d\n\r",magarc);
    }
    else {
      printf("%d.%02d\n\r",(int)(magarc/100),(int)(magarc%100));
    }
    printf("OK\n\r");
  }
  /* else if (!strcmp(argv[0],"complete")) { */
  /*   cal_complete(); */
  /*   printf("OK\n\r"); */
  /* } */
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
  for (i=0;i<CAL_SIZE;i++) {
    calibration_ram[i] = 0xFFFFFFFF;
  }
  return(0);
}

int cal_fill_test_ascending(void) {
  int i;
  int data = 100;
  for (i=0;i<CAL_SIZE;i++) {
    calibration_ram[i] = data;
    data+=100;
  }
  return(0);
}

int cal_fill_test_descending(void) {
  int i;
  int data = 20100;
  for (i=0;i<CAL_SIZE;i++) {
    calibration_ram[i] = data;
    data-=100;
  }
  return(0);
}


int cal_show_sram(void) {
  int i = 0;
  for (i=0;i<CAL_SIZE;i++) {
    printf("%d.%d,%d\n\r",
           (i+BRIGHT_MAG_X10)/10,
           (i+BRIGHT_MAG_X10)%10,
           (int)calibration_ram[i]);
  }
 
  /* for (i=1;i<CAL_SIZE;i++) { */
  /*   if (!(i%10)) { */
  /*     printf("\n\r%2d.0:",(i/10)+BRIGHT_MAG); */
  /*   } */
  /*   printf("%11d ",(int)calibration_ram[i]); */
  /* } */
  printf("OK\n\r");
  return(0);
}

int cal_f2r(void) {
  int i;
  uint32_t *p = (uint32_t *) CAL_START;
  for (i=0;i<CAL_SIZE;i++) {
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
  for (i=0;i<((CAL_SIZE/2)+(CAL_SIZE%2));i++) {
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

int cal_write(char * mag, char * value) {
  char * p = 0;  // Used to hold the whole number portion
  char * q = 0;   // Used to hold the decimal portion
  long index;
  long value_number;

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
   
  // Convert the mag to an integer
  if (q) {
    index = (strtol(mag,NULL,10)*10)+strtol(q,NULL,10) - BRIGHT_MAG_X10;
  }
  else {
    index = (strtol(mag,NULL,10)*10) - BRIGHT_MAG_X10;
  }

  // Convert the value to an integer
  if (value) {
    value_number = (strtol(value,NULL,10));
  }
  else {
    return (-1);
  }
  
  // Check to make sure the number is in range
  if ((index < 0) || index > (CAL_SIZE)) {
    return(-1);
  }
  else {
    calibration_ram[index] = value_number;
  }
  return(0);
}


int cal_lookup(uint32_t count) {
  // Function to lookup a light sensor reading in the table.
  // Assumes that the data in the table is monotonically increasing.
  // The value returned is the mag/arcsec2 * 100.
  // Value of -1 are acceptable. The function will linearly interpolate between the values
  
  int i;
  int retval = 1;
  int delta = 0;
  int last_index = -1;
 
  for (i=0;i<(CAL_SIZE-1);i++) {
    if (calibration_ram[i] == -1); // skip -1 values
    else if (count > calibration_ram[i]) {
      last_index = i; // Capture the last valid index value
    }
    else if (count < calibration_ram[i]) {
      if (last_index != -1) {
        delta = (calibration_ram[i] - calibration_ram[last_index])/((i-last_index)*10);
        count = (count-calibration_ram[last_index])/delta;
        retval = (last_index + BRIGHT_MAG_X10) * 10 + count;
        break;
      }
      else {
        // Count is less than the lowest value in the table
        retval = -1;
        break;
      }
    }
    else if (count == calibration_ram[i]) {
      // count is equal to a value in the table.
      retval = (i + BRIGHT_MAG_X10) * 10;
      break;
    }
  }
  return (retval);
}

  /*   // Acending Values */
  /*   if (calibration_ram[i] < calibration_ram[i+1]) { */
  /*     if ((count >= calibration_ram[i]) && */
  /*         (count <= calibration_ram[i+1])) { */
  /*       retval = i; */
  /*       delta = (calibration_ram[i+1]-calibration_ram[i])/10; */
  /*       count = (count-calibration_ram[i])/delta; */
  /*       retval = (retval + BRIGHT_MAG_X10) * 10 + count; // Make space for the 100ths place */
  /*       break; */
  /*     } */
  /*   } */
  /*   // Decending Values */
  /*   else if (calibration_ram[i] > calibration_ram[i+1]) { */
  /*     if ((count <= calibration_ram[i]) && */
  /*         (count >= calibration_ram[i+1])) { */
  /*       retval = i; */
  /*       delta = (calibration_ram[i]-calibration_ram[i+1])/10; */
  /*       count = (count-calibration_ram[i+1])/delta; */
  /*       if (count == 0) { */
  /*         retval = retval+1; */
  /*       } */
  /*       retval = (retval + BRIGHT_MAG_X10)*10 + count; // Multiply by 10 to make space for 100ths place.  */
  /*       break; */
  /*     } */
  /*   } */
  /*   // Exact Match  */
  /*   else { */
  /*     if (count==calibration_ram[i]) { */
  /*       retval = (i + BRIGHT_MAG_X10)*10; */
  /*       count = 0; */
  /*       break; */
  /*     } */
  /*   } */
  /* } */
  /* return(retval); */
//}

/* int cal_complete(void) { */
/*   int i; */
/*   float x1,x2,y1,y2; */
/*   float m, b; */
/*   float value; */
/*   int begin = 0; */
/*   int end = 0; */
/*   int found = 0; */
/*   int last_pass = 0; */
/*     // see of the first element in the table has a value. */
/*   begin = 0; */
/*   while (!last_pass) { */
/*     if (calibration_ram[begin]==-1) { */
/*       x1 = begin; */
/*       y1 = CAL_MIN_VALUE; */
/*       calibration_ram[begin] = CAL_MIN_VALUE; */
/*     } */
/*     else { */
/*       x1 = begin; */
/*       y1 = calibration_ram[begin]; */
/*     } */
/*     found = 0; */
/*     for (i=begin+1;i<CAL_SIZE;i++) { */
/*       if (calibration_ram[i] != -1) { */
/*         end = i; */
/*         x2=end; */
/*         y2=calibration_ram[i]; */
/*         found = 1; */
/*         break; */
/*       } */
/*     } */
/*     if (!found) { */
/*       i = CAL_SIZE; */
/*       x2 = CAL_SIZE; */
/*       y2 = CAL_MAX_VALUE; */
/*       end = CAL_SIZE-1; */
/*       calibration_ram[end] = CAL_MAX_VALUE; */
/*     } */
/*     if (i == CAL_SIZE) { */
/*       last_pass = 1; */
/*     } */
/*     /\* printf("begin=%d, end=%d\n\r",begin, end); *\/ */
/*     /\* printf("x1=%f,y1=%f  x2=%f,y2=%f\n\r",x1,y1,x2,y2); *\/ */
/*     // compute the slope */
/*     m = (y2-y1)/(x2-x1); */
    
/*     // compute the y-intercept */
/*     b = y1 - (m * x1); */
    
/*     // print the equation for the line  */
/*     /\* printf("line equation: y = %.2f*x + %.2f\n\r",m,b); *\/ */
    
/*     // Compute the intermediate points */
/*     for (i=begin+1;i<end;i++) { */
/*       value = m * (float) i + b; */
/*       calibration_ram[i] = (int) value; */
/*     } */
/*     begin = end; */
/*   } */
/*   return (0); */
/* } */

int cal_fake(void) {
  cal_blank();
  calibration_ram[1] = 1000;
  calibration_ram[10] = 3000;
  calibration_ram[20] = 6000;
  calibration_ram[50] = 13000;
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





