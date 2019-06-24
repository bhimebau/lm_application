/* command.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 04.25.2019
 * Copyright (C) 2019
 *
 */

// Command interpreter modes
// 'r' will read date from the device
// 'l' will read the log from the device
// 'a' will report the time from the device 
// 'd' will disable the alarm and prevent
//     the system from going into low
//    power modes that disable the debugger
// 't' set the RTC time
/* #define RESTART 0 */
/* #define READ_CHARACTERS 1 */
/* #define REPORT_DATA 2 */
/* #define REPORT_LOG 3 */
/* #define DEBUG 4 */
/* #define SET_TIME 5 */
/* #define REPORT_TIME 6 */

/* #define REPORT_DATA_STRING "r" */
/* #define REPORT_LOG_STRING "l" */
/* #define DEBUG_STRING "d" */
/* #define SET_TIME_STRING "t" */
/* #define REPORT_TIME_STRING "a" */

/* typedef struct light_sensor_instruction { */
/*   unsigned char command_reg;           // command that starts the instruction processing */
/*   unsigned char instruction_type;      // tells command execution phase which instruction to execute */
/*   unsigned char lla_descriptor;        // indicates the particular lla to use   */
/*   unsigned char pin_x;                 // X,Y location of the probe site to be used, LLA input pins */
/*   unsigned char pin_y; */
/*   unsigned char pin_x_alt;             // X,Y location of the probe site to be used, LLA output pins */
/*   unsigned char pin_y_alt; */
/*   int value;                           // value to be applied when in write mode. */
/*   unsigned char lla_function;     */
/*   unsigned char lla_period; */
/* } ueac_instruction_t; */
#include "queue.h"
#define MAX_COMMAND_LEN QUEUE_SIZE

typedef struct command {
  char * cmd_string;
  void (*cmd_function)(char * arg);
} command_t;

int get_command(uint8_t *);
int delspace(uint8_t *);
int execute_command(uint8_t *);
int parse_command (uint8_t *, uint8_t **, uint8_t **);
void prompt(void);




