/* rtc.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */

uint32_t pack_time(RTC_TimeTypeDef *, RTC_DateTypeDef *);
void  unpack_time(uint32_t, RTC_TimeTypeDef *, RTC_DateTypeDef *);


