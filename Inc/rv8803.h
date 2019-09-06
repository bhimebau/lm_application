/* rv8802.h
 * Description: Include file for communication with the RV-8803-C7 Real-Time Clock
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */

#ifndef RV8803_H
#define RV8803_H

#define RV8803_ADDR 0x64

typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t weekdate;
  uint8_t day;
  uint8_t month;
  uint8_t year;
} rv8803_time_date_t;

int rv8803_readreg(I2C_HandleTypeDef *, uint8_t, uint8_t *, uint16_t);
int rv8803_writereg(I2C_HandleTypeDef *, uint8_t, uint8_t *, uint16_t);
int rv8803_set_1khz_clkout(I2C_HandleTypeDef *hi2c3);
int rv8803_set_32khz_clkout(I2C_HandleTypeDef *hi2c3);
int rv8803_read_time(I2C_HandleTypeDef *, rv8803_time_date_t *);
int rv8803_write_time(I2C_HandleTypeDef *, rv8803_time_date_t *);
uint8_t bcd2dec(uint8_t);
uint8_t dec2bcd(uint8_t);
int timestruct_bcd2dec(rv8803_time_date_t * ts);
int timestruct_dec2bcd(rv8803_time_date_t * ts);

#endif

// Function Definitions

