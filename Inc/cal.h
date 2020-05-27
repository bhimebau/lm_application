/* cal.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.15.2020
 * Copyright (C) 2020
 *
 */


// Brightest and the Darkest Detectable Values
#define DARK_MAG_X10 250
#define BRIGHT_MAG_X10 190
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



