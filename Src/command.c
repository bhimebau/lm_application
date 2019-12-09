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
#include <ctype.h>
#include <string.h>
#include "strategy.h"
#include "command.h"
#include "queue.h"
#include "interrupt.h"
#include <stm32l4xx_ll_lpuart.h>



extern UART_HandleTypeDef huart1;
extern RTC_HandleTypeDef hrtc;
extern queue_t rx_queue;

void prompt(void) {
  RTC_TimeTypeDef current_time;
  RTC_DateTypeDef current_date;
  HAL_RTC_GetTime(&hrtc,&current_time,RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc,&current_date,RTC_FORMAT_BIN);
  printf("%02d/%02d/20%02d ",current_date.Month,current_date.Date,current_date.Year);
  printf("%02d:%02d:%02d ",current_time.Hours,current_time.Minutes, current_time.Seconds);
  printf("IULS> ");
}
void att_command(char *);
void ds_command(char *);           
void ts_command(char *);
void dr_command(char *);
void tr_command(char *);
void temp_command(char *);
void batt_command(char *);
void data_command(char *);
void log_command(char *);  
void erase_command(char *);
void light_command(char *);
void help_command(char *);
void version_command(char *);
void stop_command(char *);
void lof_command(char *);
void lon_command(char *);
void raw_command(char *);
void tsl237_command(char *);
void tsl237t_command(char *);
void sample_command(char *);


command_t commands[] = {
  {"@",att_command},
  {"ds",ds_command},
  {"ts",ts_command},
  {"tr",tr_command},
  {"dr",dr_command},
  {"temp",temp_command},
  {"batt",batt_command},
  {"data",data_command},
  {"log",log_command},
  {"ef",erase_command},
  {"help",help_command},
  {"ls",light_command},
  {"ver",version_command},
  {"stop",stop_command},
  {"lof",lof_command},
  {"lon",lon_command},
  {"raw",raw_command},
  {"tsl237",tsl237_command},
  {"tsl237t",tsl237t_command},
  {"sample",sample_command},
  {0,0}
};

void __attribute__((weak)) att_command(char *arguments) {
  //   printf("Attention Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
  printf("OK\n\r");
}

void __attribute__((weak)) ds_command(char *arguments) {
  printf("DS Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) ts_command(char *arguments) {
  printf("TS Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) dr_command(char *arguments) {
  printf("DR Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) tr_command(char *arguments) {
  printf("TR Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) temp_command(char *arguments) {
  printf("Temp Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) batt_command(char *arguments) {
  printf("Batt Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) data_command(char *arguments) {
  printf("Data Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) log_command(char *arguments) {
  printf("Log Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) erase_command(char *arguments) {
  printf("Erase Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) light_command(char *arguments) {
  printf("Light Sensor Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) version_command(char *arguments) {
  printf("Software Version Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) help_command(char *arguments) {
  command_t * p = commands;
  printf("Available Commands:\n\r");
  while (p->cmd_string) {
    printf("%s\n\r",p->cmd_string);
    p++;
  }
  printf("OK\n\r");
}

void __attribute__((weak)) stop_command(char *arguments) {
  printf("Led Off Default Command\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) lof_command(char *arguments) {
  printf("Led On Default Command\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) lon_command(char *arguments) {
  printf("Enter lpstop mode\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) raw_command(char *arguments) {
  printf("Display raw values for light sensor\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) tsl237_command(char *arguments) {
  printf("Display tsl237 data\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) tsl237_t_command(char *arguments) {
  printf("Display tsl237t data\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) sample_command(char *arguments) {
  printf("Take a single light sample and write it to flash\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

/* command_t commands[] = { */
/*   {"@",att_command}, */
/*   {"ds",ds_command}, */
/*   {"ts",ts_command}, */
/*   {"tr",tr_command}, */
/*   {"dr",dr_command}, */
/*   {"temp",temp_command}, */
/*   {"batt",batt_command}, */
/*   {"data",data_command}, */
/*   {"log",log_command}, */
/*   {"ef",erase_command}, */
/*   {"help",help_command}, */
/*   {0,0} */
/* }; */

int execute_command(uint8_t * line) {
  uint8_t *cmd;
  uint8_t *arg;
  command_t *p = commands;
  int success = 0;

  if (!line) {
    return (-1); // Passed a bad pointer 
  }
  if (parse_command(line,&cmd,&arg) == -1) {
    printf("Error with parse command\n\r");
    return (-1);
  }
  while (p->cmd_string) {
    if (!strcmp(p->cmd_string,(char *) cmd)) {
      if (!p->cmd_function) {
        return (-1);
      }
      (*p->cmd_function)((char *)arg);            // Run the command with the passed arguments
      success = 1;
      break;
    }
    p++;
  }
  if (success) {
    return (0);
  }
  else {
    return (-1);
  }
}

int parse_command (uint8_t *line, uint8_t **command, uint8_t **args) {
  // looks for the first comma, places a NULL and captures the remainder as the arguments
  uint8_t *p; 
  
  if((!line) ||
     (!command) ||
     (!args)) {
    return (-1); // Passed a bad pointer 
  }    
  *command = line;
  p = line;
  while (*p!=','){
    if (!*p) {
      *args = '\0';
      return(0);
    }
    p++;
  }
  *p++ = '\0'; // Replace first comma with a null
  *args = p;   // The arguments are right after the comma
  return (0);
}

int get_command(uint8_t *command_buf) {
  static uint8_t buf[QUEUE_SIZE];
  static uint32_t counter=0;
  
  uint8_t ch = 0;;
  uint32_t mask;
  uint32_t command_complete = 0;
  
  ch=dequeue(&rx_queue);
  while (ch!=0) {
      if ((ch!='\n')&&(ch!='\r')) {
        if (ch==0x7f) {               // backspace functionality
          if (counter > 0) { 
            printf("\b \b");
            counter--;
          }
        }
        /* else if (ch == 0x1b) { */
        /*   printf("up arrow\n\r"); */
        /* } */
        else {
          putchar(ch); // send the character
          while (!LL_LPUART_IsActiveFlag_TXE(LPUART1)); // wait until the character has been sent.      
          buf[counter++]=ch;
          if (counter>=(QUEUE_SIZE-2)) {
            command_complete = 1;
            break;
          }
        }
      }
      else {
        command_complete = 1;
        break;
      }
      mask = disable();
      ch=dequeue(&rx_queue);
      restore(mask);  
  }
  if (command_complete) {
    buf[counter] = 0;
    strcpy((char *)command_buf,(char *)buf);
    printf("\n\r");
    counter = 0;
    return (1);
  }
  else {
    return (0);
  }
}
    

int delspace(uint8_t *instr) {
  int length;
  uint8_t temp[MAX_COMMAND_LEN];
  uint8_t *src;
  uint8_t *dest;
  int count = 0;
  
  if (!instr) {
    return (-1); // if passed a null pointer, bail out 
  } 
  if (!(length=strnlen((char *)instr,MAX_COMMAND_LEN))) {
    return (-1); // if the passed string length is 0
  }
  if (length == MAX_COMMAND_LEN) {
    return (-1); // no null was found in the passed string. 
  }
  strcpy((char *)temp, (char *)instr);
  src = temp;                 // point at the beginning of the temp string (copy of instr)
  dest = instr;
  while(isspace(*src)) src++; // Skip leading whitespace
  while (*src!='\0') {        // traverse the source string
    if (!isspace(*src)) {     // if the character is not whitespace, copy it to the dest. 
      *dest = *src;
      dest++;
      count++;
    }
    src++;
  }
  *dest = '\0';
  return (count);
}


