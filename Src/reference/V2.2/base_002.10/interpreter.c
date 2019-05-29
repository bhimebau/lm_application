/* interpreter.c --- 
 * 
 * Filename: interpreter.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Tue Feb 28 08:55:30 2006
 * Version: 
 * Last-Updated: 
 *           By: 
 *     Update #: 0
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change log:
 * 
 * 
 */

/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* Code: */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ueac.h"
#include "interpreter.h"

#ifndef LINUX 
#include "ueaclib.h"
#include "calibrate.h"
#endif 

// String Constants 
const char DELIMITER[]=",";
const char TERMINATOR[]="\n";
const char RST[]="RST\n";
const char CAL[]="CAL\n";
const char DEBUG[]="DEBUG\n";
const char LON[]="LON\n";
const char LOF[]="LOF\n";
const char GRID_NUM_MIN[]="1";
const char GRID_NUM_MAX[]="5";

int delspace(char *instr,char *outstr) {
  int length=0;
  while (*instr!='\0') {
    if (*instr!=' ') {
      *outstr=*instr;
      outstr++;
      length++;
    }
    instr++;
  }
  *outstr='\0';
  return length;
}

// Takes a newline delimited string and modifies the ueac's system state
int interpreter (char *command_string,ueac_t *system_state) {
  enum {COMMAND,SPECIFIER,F3,F4,F5,F6,F7,F8,F9,F10};           // field states
  enum {R_PRIMITIVE,W_PRIMITIVE,PRINTGRID,LLA,GLOBAL_CONTROL};  // instruction type
  enum {NOT_DONE,DONE,FAILED};

  char *token = NULL;
  unsigned char error_code = 0;
  int field = COMMAND;                                    // State variable to indicated which delimited 
  int type = R_PRIMITIVE;                                 // State varible to indicate the context of the field
  char *strptr=NULL;
  char tokbuf[20];                                        // field buffer used to strip spaces from the input
  int command_complete = NOT_DONE;                        // Variable to indicate if the command string has an issue 
  char *tokend;
  long result_temp;

  while (system_state->instruction.command_reg==UEAC_EXECUTE); // wait while the last command completes
  token = strtok(command_string,DELIMITER);                    // grab the next token and pass it to the field parser  
  while ((token!=NULL)&&(command_complete!=FAILED)) {          // stop when strtok returns NULL or parsing fails
    delspace(token,tokbuf);
    switch (field) {
    // Parse the individual fields
    case COMMAND:   
      if (!strcasecmp(tokbuf,"R")) {
        type = R_PRIMITIVE;
      }
      else if (!strcasecmp(tokbuf,"W")) {
        type = W_PRIMITIVE;
      }
      else if (!strcasecmp(tokbuf,"P")) {
        type = PRINTGRID;
      }
      else if (!strcasecmp(tokbuf,"L")) {
        type = LLA;
      }
      else if (!strcasecmp(tokbuf,RST)) {
        system_state->instruction.instruction_type=UEAC_RST;
        command_complete=DONE;
      }
      else if (!strcasecmp(tokbuf,CAL)) {
        system_state->instruction.instruction_type=UEAC_CAL;
        command_complete=DONE;
      }
      else if (!strcasecmp(tokbuf,DEBUG)) {
        system_state->instruction.instruction_type=UEAC_DEBUG;
        command_complete=DONE;
      }
      else if (!strcasecmp(tokbuf,LON)) {
        system_state->instruction.instruction_type=UEAC_LON;
        command_complete=DONE;
      }
      else if (!strcasecmp(tokbuf,LOF)) {
        system_state->instruction.instruction_type=UEAC_LOF;
        command_complete=DONE;
      }
      else {
        command_complete=FAILED;
        error_code=1;
      }
      field=SPECIFIER;
      break;
    case SPECIFIER:
      switch (type) {
        // Parse field based on specifics of the particular command type
      case R_PRIMITIVE:
        if (!strcasecmp(tokbuf,"V")) {
          system_state->instruction.instruction_type=UEAC_READ_V;
          field = F3;
        }
        else if (!strcasecmp(tokbuf,"I")) {
          system_state->instruction.instruction_type=UEAC_READ_I;
          field = F3;
        }
        else {
          command_complete=FAILED;
          error_code=2;
        }
        break;
      case W_PRIMITIVE:
        if (!strcasecmp(tokbuf,"I")) {
          system_state->instruction.instruction_type=UEAC_WRITE;
          field = F3;
        }
        else {
          command_complete=FAILED;
          error_code=3;
        }
        break;
      case PRINTGRID:
        if ((strptr=strchr(tokbuf,'\n'))==NULL) {
          // newline not found which is an error in this field. 
          command_complete=FAILED;
          error_code=4;
          break;
        }
        else {
          *strptr=0; // since newline found, strip it from tokbuf for V/I match
        }
        // Parse the field now that the newline has been located
        if (!strcasecmp(tokbuf,"V")) {
          system_state->instruction.instruction_type=UEAC_ALL_V;
          command_complete=DONE;
        }
        else if (!strcasecmp(tokbuf,"I")) {
          system_state->instruction.instruction_type=UEAC_ALL_I;
          command_complete=DONE;
        }
        else {
         command_complete=FAILED;
         error_code=5;
        }
        break;
      case LLA:
        if (!strcasecmp(tokbuf,"A")) {
          system_state->instruction.instruction_type=UEAC_LLA_ADD;
          field = F3;
        }
        else if (!strcasecmp(tokbuf,"D")) {
          system_state->instruction.instruction_type=UEAC_LLA_DISABLE;
          field = F3;
        }
        else if (!strcasecmp(tokbuf,"E")) {
          system_state->instruction.instruction_type=UEAC_LLA_ENABLE;
          field = F3;
        }
        else if (!strcasecmp(tokbuf,"R")) {
          system_state->instruction.instruction_type=UEAC_LLA_REPORT;
          field = F3;
        }
        else {
         command_complete=FAILED;
         error_code=5;
        }
      }
      break;
    case F3:
      switch (type) {
      case R_PRIMITIVE:
      case W_PRIMITIVE:  
      case LLA:
        // For RW/LLA this should be the X location (XIN)
        result_temp=strtol(tokbuf,&tokend,10);
        if (*tokend!=0) {
          command_complete=FAILED;
          error_code=6;
        }
        else {
          if ((result_temp>=1)&&(result_temp<=5)) {
            system_state->instruction.pin_x=(unsigned char) result_temp;
            field=F4;
          }
          else {
            command_complete=FAILED;
            error_code=6;
          }
        }
        break;
      }
      break;
    case F4:
      switch (type) {
      case R_PRIMITIVE:
      case W_PRIMITIVE:
      case LLA:
        // For RW/LLA this should be the Y location (LLA YIN)
        result_temp=strtol(tokbuf,&tokend,10);
        if (*tokend!=0) {
          command_complete=FAILED;
          error_code=6;
        }
        else {
          if ((result_temp>=1)&&(result_temp<=5)) {
            system_state->instruction.pin_y=(unsigned char) result_temp;
            field=F5;
          }
          else {
            command_complete=FAILED;
            error_code=9;
          }
        }
        break;
      }
      break;
    case F5:
      switch (type) {
      case R_PRIMITIVE:
      case W_PRIMITIVE:
        // For RW this should be the value field
        if ((strptr=strchr(tokbuf,'\n'))==NULL) {
          command_complete=FAILED; // lack of newline is an error in this field. 
          error_code=10;
          break;
        }
        else {
          *strptr=0; // since newline found, strip it from tokbuf for V/I match
        }
        result_temp=strtol(tokbuf,&tokend,10);
        if (*tokend!=0) {
          command_complete=FAILED;
          error_code=6;
        }
        else {
          if ((result_temp<=W_VALUE_MAX)&&(result_temp>=W_VALUE_MIN)) {
            system_state->instruction.value=result_temp;
            command_complete=DONE;
          }
          else {
            command_complete=FAILED;
            error_code=12;
          }
        }
        break;
      case LLA:
        // For LLA this should be the X Output location
        result_temp=strtol(tokbuf,&tokend,10);
        if (*tokend!=0) {
          command_complete=FAILED;
          error_code=6;
        }
        else {
          if ((result_temp>=0)&&(result_temp<=5)) {
            system_state->instruction.pin_x_alt=(unsigned char) result_temp;
            field=F6;
          }
          else {
            command_complete=FAILED;
            error_code=9;
          }
        }
        break;
      }
      break;
    case F6:
      switch (type) {
      case LLA:
        // For LLA this should be the Y Output location
        result_temp=strtol(tokbuf,&tokend,10);
        if (*tokend!=0) {
          command_complete=FAILED;
          error_code=6;
        }
        else {
          if ((result_temp>=0)&&(result_temp<=5)) {
            system_state->instruction.pin_y_alt=(unsigned char) result_temp;
            field=F7;
          }
          else {
            command_complete=FAILED;
            error_code=9;
          }
        }
        break;
      }
      break;
    case F7:
      switch (type) {
      case LLA:
        // For LLA commands this is the numeric descriptor for the LLA
        result_temp=strtol(tokbuf,&tokend,10);
        if (*tokend!=0) {
          command_complete=FAILED;
          error_code=6;
        }
        else {
          if ((result_temp>0)&&(result_temp<=10)) {
            system_state->instruction.lla_descriptor=(unsigned char) result_temp;
            field=F8;
          }
          else {
            command_complete=FAILED;
            error_code=6;
          }
        }
        break;
      }
      break;
    case F8:
      switch (type) {
      case LLA:
        // For LLA commands this is the lla_function
        result_temp=strtol(tokbuf,&tokend,10);
        if (*tokend!=0) {
          command_complete=FAILED;
          error_code=6;
        }
        else {
          if ((result_temp>0)&&(result_temp<=52)) {
            system_state->instruction.lla_function=(unsigned char) result_temp;
            field=F9;
          }
          else {
            command_complete=FAILED;
            error_code=6;
          }
        }
        break;
      }
      break;
    case F9:
        // For LLA commands this is the lla_period
      switch (type) {
      case LLA:
        // For LLA 
        if ((strptr=strchr(tokbuf,'\n'))==NULL) {
          command_complete=FAILED; // lack of newline is an error in this field. 
          error_code=10;
          break;
        }
        else {
          *strptr=0; // since newline found, strip it from tokbuf for V/I match
        }
        result_temp=strtol(tokbuf,&tokend,10);
        if (*tokend!=0) {
          command_complete=FAILED;
          error_code=6;
        }
        else {
          if ((result_temp>0)&&(result_temp<=255)) {
            system_state->instruction.lla_period=(unsigned char) result_temp;
            command_complete=DONE;
          }
          else {
            command_complete=FAILED;
            error_code=6;
          }
        }
        break;
      }
      break;
    case F10:
      if (!strncasecmp(tokbuf,TERMINATOR,sizeof(TERMINATOR))) {
        printf("Found Terminator\n");
      }
      else {
        printf("F10 %s\n",tokbuf);
      }
      break;
    }
    token = strtok(NULL,DELIMITER);  // get the next token 
  }
  if (command_complete==DONE) {
    system_state->instruction.command_reg=UEAC_EXECUTE;
    ueac_execute_instruction(system_state);
    while (system_state->instruction.command_reg==UEAC_EXECUTE); // wait while the last command completes
  }
  else {
#ifndef LINUX 
    printf("NOK\n\r");
#else
    printf("NOK\n");
#endif 
  }
  return error_code;
}

/* interpreter.c ends here */

