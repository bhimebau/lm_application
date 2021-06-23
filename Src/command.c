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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "strategy.h"
#include "command.h"
#include "queue.h"
#include "interrupt.h"
#include "skydata.h"
#include <stm32l4xx_ll_lpuart.h>
extern ADC_HandleTypeDef hadc1;
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
void data_command(char *);
void log_command(char *);  
void erase_command(char *);
void help_command(char *);
void version_command(char *);
void tsl237_command(char *);

/* void lof_command(char *); */
/* void lon_command(char *); */
void led_command(char *);
void raw_command(char *);
void sample_command(char *);
void debug_command(char *);
void flash_command(char *);
void uid_command(char *);
void cal_command(char *);
void batt_command(char *);
void sky_command(char *);
void temp_command(char *);
void sample_frequency_command(char *);


command_t commands[] = {
  {"@",att_command},
  {"ds",ds_command},
  {"ts",ts_command},
  {"tr",tr_command},
  {"dr",dr_command},
  {"data",data_command},
  {"log",log_command},
  {"ef",erase_command},
  {"help",help_command},
  {"ver",version_command},
  {"tsl237",tsl237_command},
  /* {"lof",lof_command}, */
  /* {"lon",lon_command}, */
  {"led",led_command},
  {"sample",sample_command},
  {"debug",debug_command},
  {"flash",flash_command},
  {"uid",uid_command},
  {"cal",cal_command},
  {"sky",sky_command},
  {"temp",temp_command},
  /* {"batt",batt_command}, */
  {"sampfreq",sample_frequency_command},
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

/* void __attribute__((weak)) batt_command(char *arguments) { */
/*   printf("Batt Command Processing\n\r"); */
/*   if (arguments) { */
/*     printf("Arguments = %s\n\r",arguments); */
/*   } */
/* } */

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
  printf("LP Stop Default Command\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

/* void __attribute__((weak)) lof_command(char *arguments) { */
/*   printf("Led On Default Command\n\r"); */
/*   if (arguments) { */
/*     printf("Arguments = %s\n\r",arguments); */
/*   } */
/* } */

/* void __attribute__((weak)) lon_command(char *arguments) { */
/*   printf("Enter lpstop mode\n\r"); */
/*   if (arguments) { */
/*     printf("Arguments = %s\n\r",arguments); */
/*   } */
/* } */

void __attribute__((weak)) led_command(char *arguments) {
  printf("Led Drive Default Command\n\r");
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

void __attribute__((weak)) debug_command(char *arguments) {
  printf("Enable or Disable STOP2 debugging\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) flash_command(char *arguments) {
  printf("Report Flash Meta Data\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) cal_command(char *arguments) {
  printf("Build and report on the sensor calibration command\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) sky_command(char *arguments) {
  printf("write the LED to a specific dark sky value\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void __attribute__((weak)) sample_frequency_command(char *arguments) {
  printf("Set the sample frequency of the system\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

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
      /* HAL_ADC_Init(&hadc1); */
      /* // Calibrate the A2D */
      /* while (HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED) != HAL_OK); // must be done on each init of the converter.  */
      (*p->cmd_function)((char *)arg);            // Run the command with the passed arguments
      /* HAL_ADC_DeInit(&hadc1); */
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

enum {COLLECT_CHARS, ESCAPE_SEQUENCE, COMPLETE};
enum {ESC, LBRACKET};
  
int get_command(uint8_t *command_buf) {
  static uint8_t buf[QUEUE_SIZE] = {0};
  static uint8_t last[QUEUE_SIZE] = {0};
  static uint32_t counter=0;
  static uint32_t mode = COLLECT_CHARS;
  static uint32_t esmode = ESC;
    
  uint8_t ch = 0;;
  uint32_t mask;
  uint32_t len;
  uint32_t i;
  // uint32_t command_complete = 0;
  
  ch=dequeue(&rx_queue);
  while (ch!=0) {
    switch(mode) {
    case COLLECT_CHARS:
      if ((ch!='\n')&&(ch!='\r')) {
        if (ch==0x7f) {               // backspace functionality
          if (counter > 0) { 
            printf("\b \b");
            counter--;
          }
        }
        else if (ch == 0x1b) {
          mode = ESCAPE_SEQUENCE;
          esmode = ESC;
        }
        else {
          putchar(ch); // send the character
          while (!LL_LPUART_IsActiveFlag_TXE(LPUART1)); // wait until the character has been sent.      
          buf[counter++]=ch;
          if (counter>=(QUEUE_SIZE-2)) {
            mode=COMPLETE;
            break;
          }
        }
      }
      else {
        mode = COMPLETE;
        break;
      }
      break;
    case ESCAPE_SEQUENCE:
      switch(esmode) {
      case ESC:
        if (ch == '[') {
          esmode = LBRACKET;
        }
        else {
          mode = COLLECT_CHARS;
        }
        break;
      case LBRACKET:
        if (ch == 'A') {
          if ((len = strlen((char *)last))) {
            while (counter>0) {
              // Erase any characters currently on the command line 
              putchar('\b');
              counter--;
            }
            for (i=0;i<len;i++) {
              // show the last command
              putchar(last[i]);
            }
            strcpy((char *)buf,(char *)last); // copy the last command into the current command buffer
            counter = len;
          }
        }
        mode = COLLECT_CHARS;
        break;
      }
      break;
    }
    mask = disable();
    ch=dequeue(&rx_queue);
    restore(mask);  
  }
  if (mode == COMPLETE) {
    buf[counter] = 0;
    strcpy((char *)command_buf,(char *)buf);
    strcpy((char *)last,(char *)buf);
    printf("\n\r");
    counter = 0;
    mode = COLLECT_CHARS;
    return(1);
  }
  else {
    return(0);
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


