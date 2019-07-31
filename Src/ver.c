/* version.c
 * Description: prints the software version 
 *
 * Author: Bryce Himebaugh  * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */
#include "main.h"
#include <stdio.h>

void version_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    printf("%s\n\r",VERSION);
    printf("OK\n\r");
  }
}
