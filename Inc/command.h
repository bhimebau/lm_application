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
#define RESTART 0
#define READ_CHARACTERS 1
#define REPORT_DATA 2
#define REPORT_LOG 3
#define DEBUG 4
#define SET_TIME 5
#define REPORT_TIME 6

#define REPORT_DATA_STRING "r"
#define REPORT_LOG_STRING "l"
#define DEBUG_STRING "d"
#define SET_TIME_STRING "t"
#define REPORT_TIME_STRING "a"
