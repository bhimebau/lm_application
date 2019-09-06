/* rv8803.c
 * Description: Functions used to communicate with the RV-8803-C7 Real-Time Clock. 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <stdio.h>
#include "rv8803.h"

int rv8803_readreg(I2C_HandleTypeDef *hi2c3, uint8_t addr, uint8_t * data, uint16_t size) {
  HAL_StatusTypeDef status;
  status = HAL_I2C_Mem_Read(hi2c3,
                   0x65,
                   addr,
                   1,
                   data,
                   size,
                   HAL_MAX_DELAY
                   );
  return((int) status);
}

int rv8803_writereg(I2C_HandleTypeDef *hi2c3, uint8_t addr, uint8_t * data, uint16_t size) {
  HAL_StatusTypeDef status;
  status = HAL_I2C_Mem_Write(hi2c3,
                            0x64,
                            addr,
                            1,
                            data,
                            size,
                            HAL_MAX_DELAY
                            );
  return((int) status);
}

int rv8803_set_1khz_clkout(I2C_HandleTypeDef *hi2c3) {
  uint8_t dataval = 0x04; // Sets the TD field to 01 which makes clkout 1024hz
  rv8803_writereg(hi2c3,0x0d,&dataval,1);
  return (0);
}

int rv8803_set_32khz_clkout(I2C_HandleTypeDef *hi2c3) {
  uint8_t dataval = 0x00; // Sets the TD field to 01 which makes clkout 1024hz
  rv8803_writereg(hi2c3,0x0d,&dataval,1);
  return (0);
}

int rv8803_read_time(I2C_HandleTypeDef *hi2c3, rv8803_time_date_t * timestruct) {
  rv8803_readreg(hi2c3,0x00,(uint8_t *) timestruct,7); // Copies the time from the rv8803.
  timestruct_bcd2dec(timestruct);
  return (0);
}

int rv8803_write_time(I2C_HandleTypeDef *hi2c3, rv8803_time_date_t * timestruct) {
  timestruct_dec2bcd(timestruct);
  rv8803_writereg(hi2c3,0x00,(uint8_t *) timestruct,7); // Copies the timestruct to the rv8803. 
  return (0);
}


uint8_t bcd2dec(uint8_t bcd) {
  uint8_t conversion = 0;
  conversion = (bcd & 0xF0) >> 4;
  conversion *= 10;
  conversion += (bcd & 0x0F);
  return(conversion);
}       

uint8_t dec2bcd(uint8_t decimal) {
  uint8_t conversion = 0;
  conversion = (decimal/10) << 4;
  conversion = conversion | (decimal%10);
  return(conversion);
}

int timestruct_bcd2dec(rv8803_time_date_t * ts) {
  if (!ts) return (-1);
  ts->second = bcd2dec(ts->second);
  ts->minute = bcd2dec(ts->minute);
  ts->hour = bcd2dec(ts->hour);
  ts->weekdate = bcd2dec(ts->weekdate);
  ts->day = bcd2dec(ts->day);
  ts->month = bcd2dec(ts->month);
  ts->year = bcd2dec(ts->year);
  return 0;
}

int timestruct_dec2bcd(rv8803_time_date_t * ts) {
  if (!ts) return(-1);
  ts->second = dec2bcd(ts->second);
  ts->minute = dec2bcd(ts->minute);
  ts->hour = dec2bcd(ts->hour);
  ts->weekdate = dec2bcd(ts->weekdate);
  ts->day = dec2bcd(ts->day);
  ts->month = dec2bcd(ts->month);
  ts->year = dec2bcd(ts->year);
  return 0;
}
