/* flash.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 04. 7.2019
 * Copyright (C) 2019
 *
 */

#ifndef FLASH_H
#define FLASH_H

// STM32L432 Flash Starts at 0x8000000 and ends at 0x803FFFFF. 
#define FLASH_START        0x08000000
// Last 16 byte word in the memory is 0x0803FFF0 
#define FLASH_END          0x0803FFF0
#define SENTINEL_MARK_BOTTOM      0xDEADBEEFA5A5A5A5
#define SENTINEL_MARK_TOP         0xFEEDC0DE5A5A5A5A

#define DATA_RECORD 1
#define LOG_RECORD 2
#define ALL_RECORD 3

typedef struct sensordata {
  uint8_t watermark;               // 0x01=populated, 0xFF=unpopulated 
  uint8_t status;                  // record type, 01=sensor data, 02=error data;
  uint16_t record_number;          // Which number is this particular record  
  uint32_t timestamp;              // Time, bit packed into 32 bits
  uint16_t battery_voltage;        // 16 bit battery voltage
  uint16_t temperature;            // STM32 Temperature sensor reading 
  float lux;                       // Reading from the light sensor
} sensordata_t;

typedef struct log_data {
  uint8_t watermark;               // 0x01=populated, 0xFF=unpopulated 
  uint8_t status;                  // record type, 01=sensor data, 02=error data;
  uint16_t record_number;          // Which number is this particular record  
  uint32_t timestamp;              // Time, bit packed into 32 bits
  uint8_t msg[8];                  // String message to make the record seem less cryptic.
} logdata_t;

typedef struct raw {
  uint64_t data0;
  uint64_t data1;
} raw_t;

typedef struct flash_status {
  uint64_t * data_start;
  uint32_t total_records;
  uint32_t max_possible_records;
  uint64_t * next_address;
  uint32_t next_record_number;
} flash_status_t;
 
#endif

uint64_t *find_sentinel_bottom(void);
uint64_t *find_sentinel_top(void);
int write_sentinel(uint64_t *, raw_t *);
int flash_write_init(flash_status_t *);
int write_record(flash_status_t *, void *);
int read_all_records(flash_status_t *, int);
int write_sensor_data(flash_status_t *,uint16_t,uint16_t,float);
int write_log_data(flash_status_t *, char *);
int report_flash_status(flash_status_t *);
