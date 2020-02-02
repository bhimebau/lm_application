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
#define DARK_MAG 25
#define BRIGHT_MAG 5
#define CAL_MAX_INDEX ((DARK_MAG-BRIGHT_MAG)*10+1)

int flash_caldata(int, caldata_t *);
int cal_blank(void);
int cal_show_sram(void);
int cal_f2r(void);
int cal_r2f(void);
int cal_write(char *,uint32_t);


