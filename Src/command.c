/* command.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 04.25.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <stdio.h>
#include <string.h>
#include "strategy.h"
#include "command.h"

extern UART_HandleTypeDef huart1;

int interpreter(system_state_t *sys_state) {
  unsigned char ch;
  int status;
  switch(sys_state->communication_state) {
  case RESTART:
    printf("\n\rlight_sensor> ");
    sys_state->pos = sys_state->command_buf;
    sys_state->communication_state = READ_CHARACTERS;
    break;
  case READ_CHARACTERS:
    while ((status=HAL_UART_Receive(&huart1, &ch, 1, 1)) == HAL_OK) {
      if ((ch!='\n') || (ch!='\r')) {
        // write the character to the buffer 
        *(sys_state->pos) = ch;
        sys_state->pos++;
      }
      else {
         // Add a NULL character to the make the buffer a string
        *(sys_state->pos) = '\0';
        if (!strncmp(sys_state->command_buf,REPORT_DATA_STRING,MAXBUF)) {
          sys_state->communication_state = REPORT_DATA;
        }
        else if (!strncmp(sys_state->command_buf,REPORT_LOG_STRING,MAXBUF)) {
          sys_state->communication_state = REPORT_LOG;
        }
        else if (!strncmp(sys_state->command_buf,DEBUG_STRING,MAXBUF)) {
          sys_state->communication_state = DEBUG;
        }
        else if (!strncmp(sys_state->command_buf,SET_TIME_STRING,MAXBUF)) {
          sys_state->communication_state = SET_TIME;
        }
        else if (!strncmp(sys_state->command_buf,REPORT_TIME_STRING,MAXBUF)) {
          sys_state->communication_state = REPORT_TIME;
        }
        else {
          printf("Unknown Command");
          sys_state->communication_state = RESTART;
        }
      }
    }
    break;
  case REPORT_DATA:
    printf("Report the data\n\r");
    sys_state->communication_state = RESTART;
    break;
  case REPORT_LOG:
    printf("Report the log\n\r");
    sys_state->communication_state = RESTART;
    break;
  case DEBUG:
    printf("Debug Mode\n\r");
    sys_state->communication_state = RESTART;
    break;
  case SET_TIME:
    printf("Set the time\n\r");
    sys_state->communication_state = RESTART;
    break;
  case REPORT_TIME:
    printf("Report Time\n\r");
    sys_state->communication_state = RESTART;
    break;
  }
  return (0);
}

