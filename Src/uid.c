/* uid.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.21.2020
 * Copyright (C) 2020
 *
 */

#include "main.h"
#include <stdio.h>

void uid_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    printf("%08X%08X%08X\n\r",
           (unsigned int) HAL_GetUIDw0(),
           (unsigned int) HAL_GetUIDw1(),
           (unsigned int) HAL_GetUIDw2());
    printf("OK\n\r");
  }
}
