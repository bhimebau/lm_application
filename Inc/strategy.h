/* strategy.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 04.24.2019
 * Copyright (C) 2019
 *
 */

// Accept commands in RUN mode for 30 seconds
// The alarm will be set to end this mode.
// Receiving a D 
// 
// Wait for a serial port command
#define COMMAND 0

// STOP2 Mode for 1 hour interval
#define HOUR_STOP 1

// Start a sensor read operation
// STOP2 for 1 second to wait for sensor
#define START_SENSOR 3

// Collect the data from the sensor
// If the data is 0, go back to start sensor
#define READ_SENSOR 4

// Write the lux data to the flash
#define WRITE_RECORD 5

// Once the flash is full, drop into the state
// Disable RTC alarm, drop into standby
// Only a reset will wake up the processor. 
#define MEMORY_FULL 6

#define MAXBUF 30

typedef struct system_state {
  int operating_state;
  int communication_state;
  char command_buf[MAXBUF];
  char *pos;
  int memory_status;
} system_state_t;

  
