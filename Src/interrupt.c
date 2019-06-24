/* interrupt.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 06.21.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include "interrupt.h"


uint32_t disable(void) {
  int32_t priority_mask = 0;
  priority_mask = __get_PRIMASK(); // Read the current state of the interrupt mask
  __disable_irq();                // Globally mask the interrupts off
  return (priority_mask);
}

void restore(uint32_t priority_mask) {
  __set_PRIMASK(priority_mask);
}

void enable(void) {
  __enable_irq();
}

  
