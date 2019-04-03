/* tls25911.c
 * Description: Functions used to communicate with the TLS25911 light sensor. 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <stdio.h>
#include "tsl25911.h"

// The power supply VSW must be enabled through the SW_MODE pin before the TSL25911 will be
// powered. 

#define LIGHT_SENSOR_ADDRESS (0x29<<1)
#define ID_REG 0x12

uint8_t tsl2561_readid(I2C_HandleTypeDef *hi2c1) {
  uint8_t rxbuf;
  HAL_I2C_Mem_Read(hi2c1,
                   LIGHT_SENSOR_ADDRESS,
                   0xA0|ID_REG,
                   1,
                   (uint8_t *) &rxbuf,
                   1,
                   HAL_MAX_DELAY
                   );
  return rxbuf;
}
