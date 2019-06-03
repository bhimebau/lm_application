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

extern UART_HandleTypeDef huart1;

void att_command(char *arguments) {
  printf("Attention Command Processing\n\r");
  printf("Arguments = %s\n\r",arguments);
}

void ds_command(char *arguments) {
  printf("DS Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void ts_command(char *arguments) {
  printf("TS Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void dr_command(char *arguments) {
  printf("DR Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void tr_command(char *arguments) {
  printf("TR Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void temp_command(char *arguments) {
  printf("Temp Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void batt_command(char *arguments) {
  printf("Temp Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void data_command(char *arguments) {
  printf("Data Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

void erase_command(char *arguments) {
  printf("Erase Command Processing\n\r");
  if (arguments) {
    printf("Arguments = %s\n\r",arguments);
  }
}

command_t commands[] = {
  {"@",att_command},
  {"ds",ds_command},
  {"ts",ts_command},
  {"tr",tr_command},
  {"dr",dr_command},
  {"temp",temp_command},
  {"batt",batt_command},
  {"data",data_command},
  {"ef",erase_command},
  {0,0}
};

int execute_command(char * line) {
  char *cmd;
  char *arg;
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
    if (!strcmp(p->cmd_string,cmd)) {
      if (!p->cmd_function) {
        return (-1);
      }
      (*p->cmd_function)(arg);            // Run the command with the passed arguments
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

int parse_command (char *line, char **command, char **args) {
  // looks for the first comma, places a NULL and captures the remainder as the arguments
  char *p; 
  
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

int get_command(char *command_buf) {
  char ch;
  int counter=0;
  while(((ch=getchar())!='\n')&&(ch!='\r')&&(counter++<MAX_COMMAND_LEN)) {
    putchar(ch);
    *command_buf++=ch;
  }
  *command_buf=0;
  printf("\n\r");
  return (counter);
}    

int delspace(char *instr) {
  int length;
  char temp[MAX_COMMAND_LEN];
  char *src;
  char *dest;
  int count = 0;
  
  if (!instr) {
    return (-1); // if passed a null pointer, bail out 
  } 
  if (!(length=strnlen(instr,MAX_COMMAND_LEN))) {
    return (-1); // if the passed string length is 0
  }
  if (length == MAX_COMMAND_LEN) {
    return (-1); // no null was found in the passed string. 
  }
  strcpy(temp, instr);
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


