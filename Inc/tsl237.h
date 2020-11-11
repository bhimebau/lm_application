/* tsl237.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 09.11.2019
 * Copyright (C) 2019
 *
 */

#ifndef TSL237_H
#define TSL237_H

#endif

// Only allow 5 seconds for taking 1000 samples
// #define MAX_SAMPLE_TIME 5000

// Testing value to see if this is why the sensor is not seeing sky values greater than 21. 
#define MAX_SAMPLE_TIME 480000

extern uint32_t tsl237_break_received;

extern uint32_t tsl237_done;
extern uint32_t tsl237t_done;

void tsl237_vdd_on(void);
void tsl237_vdd_off(void);
void tsl237_command(char *);
// float tsl237_readsensor(void);
uint32_t tsl237_readsensor(void); 
