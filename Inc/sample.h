/* sample.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.13.2020
 * Copyright (C) 2020
 *
 */

typedef struct sample_control {
  int enable;
  int interval_hours;
  int interval_minutes;
  int begin_hour;
  int end_hour;
} sample_control_t;
  
void sample(void);
uint32_t sample_noflash();
