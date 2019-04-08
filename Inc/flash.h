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
#define FLASH_END            0x0803FFFC
#define SENTINEL_MARK        0xDEADBEEFA5A5A5A5

#endif

uint32_t * find_flash_start(void);
void test_flash(void);
