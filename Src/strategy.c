/* strategy.c
 * Description: Light sensor collection algorithm. 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 04.24.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <stdio.h>
#include "strategy.h"

int strategy(system_state_t *sys_state) {
  switch(sys_state->operating_state) {
  case COMMAND:
    break;
  case HOUR_STOP:
    break;
  case START_SENSOR:
    break;
  case READ_SENSOR:
    break;
  case WRITE_RECORD:
    break;
  case MEMORY_FULL:
    break;
  default:
    return (0);
  }
  return (0);
}




