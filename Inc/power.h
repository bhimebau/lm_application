/* power.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 06.24.2019
 * Copyright (C) 2019
 *
 */

#define POWER_OFF 0
#define POWER_ON 1

void lp_stop_wfi(void);
int sensor_power(uint32_t state);
