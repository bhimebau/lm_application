/* cal.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.15.2020
 * Copyright (C) 2020
 *
 */

#include "skydata.h"

// Brightest and the Darkest Detectable Values
/* #define DARK_MAG_X10 250 */
/* #define BRIGHT_MAG_X10 190 */
#define DARK_MAG_X10 DARKEST_VALUE_ALLOWED
#define BRIGHT_MAG_X10 BRIGHTEST_VALUE_ALLOWED

#define CAL_SIZE (DARK_MAG_X10-BRIGHT_MAG_X10)
// #define CAL_MAX_VALUE 60000000
// #define CAL_MIN_VALUE 200000

int flash_caldata(int, caldata_t *);
int cal_blank(void);
int cal_fill_test_ascending(void);
int cal_fill_test_descending(void);
int cal_show_sram(void);
int cal_f2r(void);
int cal_r2f(void);
int cal_write(char *,char *);
int cal_lookup(uint32_t);
int cal_complete(void);
int cal_fake(void);
int cal_offset(char *, char *);
int cal_temp(char *, char *);
int flash_cal_offset(offset_t *);
int flash_cal_temperature(tempdata_t * t);
int cal_show_conditions(void);
uint32_t cal_sample_temperature_compensation(uint32_t, int32_t);
int cal_compensate_magnitude(int);
