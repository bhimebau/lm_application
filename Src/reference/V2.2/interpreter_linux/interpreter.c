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

// String Constants 
const char DELIMITER[]=",";
const char TERMINATOR[]="\n";
const char R[]="R";
const char W[]="W";
const char P[]="P";
const char V[]="V";
const char I[]="I";
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
  enum {R_PRIMITIVE,W_PRIMITIVE,PRINTGRID,LLA_INSTANTIATION,LLA_TABLE,GLOBAL_CONTROL};  // instruction type
  enum {NOT_DONE,DONE,FAILED};

  char *token = NULL;
  unsigned char error_code = 0;
  int field = COMMAND;                                    // State variable to indicated which delimited 
  int type = R_PRIMITIVE;                                 // State varible to indicate the context of the field
  char *strptr=NULL;
  char tokbuf[20];                                        // field buffer used to strip spaces from the input
  int command_complete = NOT_DONE;                        // Variable to indicate if the command string has an issue 
  int value_temp=0;

  while (system_state->instruction.command_reg==UEAC_EXECUTE); // wait while the last command completes

  token = strtok(command_string,DELIMITER);               // grab the next token and pass it to the field parser  
  while ((token!=NULL)&&(command_complete!=FAILED)) {     // stop when strtok returns NULL or parsing fails
    delspace(token,tokbuf);                               // strip any spaces from the field token
    switch (field) {
    // Parse the individual fields
    case COMMAND:   
      if (!strncasecmp(tokbuf,R,sizeof(R))) {
        type = R_PRIMITIVE;
      }
      else if (!strncasecmp(tokbuf,W,sizeof(W))) {
        type = W_PRIMITIVE;
      }
      else if (!strncasecmp(tokbuf,P,sizeof(P))) {
        type = PRINTGRID;
      }
      else {
        command_complete=FAILED;
      }
      field=SPECIFIER;
      break;
    case SPECIFIER:
      switch (type) {
        // Parse field based on specifics of the particular command type
      case R_PRIMITIVE:
        if (!strncasecmp(tokbuf,V,sizeof(V))) {
          system_state->instruction.instruction_type=UEAC_READ_V;
          field = F3;
        }
        else if (!strncasecmp(tokbuf,I,sizeof(I))) {
          system_state->instruction.instruction_type=UEAC_READ_I;
          field = F3;
        }
        else {
          command_complete=FAILED;
        }
        break;
      case W_PRIMITIVE:
        if (!strncasecmp(tokbuf,I,sizeof(I))) {
          system_state->instruction.instruction_type=UEAC_WRITE;
          field = F3;
        }
        else {
          command_complete=FAILED;
        }
        break;
      case PRINTGRID:
        if ((strptr=strchr(tokbuf,'\n'))==NULL) {
          // newline not found which is an error in this field. 
          command_complete=FAILED;
          break;
        }
        else {
          *strptr=0; // since newline found, strip it from tokbuf for V/I match
        }
        // Parse the field now that the newline has been located
        if (!strncasecmp(tokbuf,V,sizeof(V))) {
          system_state->instruction.instruction_type=UEAC_ALL_V;
          command_complete=DONE;
        }
        else if (!strncasecmp(tokbuf,I,sizeof(I))) {
          system_state->instruction.instruction_type=UEAC_ALL_I;
          command_complete=DONE;
        }
        else {
         command_complete=FAILED;
        }
        break;
      }
      break;
    case F3:
      switch (type) {
      case R_PRIMITIVE:
      case W_PRIMITIVE:  
        // For RW this should be the X location
        if (strlen(tokbuf)!=1) {
          command_complete=FAILED;
        }
        else {
          if ((strncasecmp(tokbuf,GRID_NUM_MIN,sizeof(GRID_NUM_MIN))>=0) && \
              (strncasecmp(tokbuf,GRID_NUM_MAX,sizeof(GRID_NUM_MIN))<=0)) {
            system_state->instruction.pin_x=(unsigned char) atoi(tokbuf);
            field=F4;
          }
          else {
            command_complete=FAILED;
          }
        }
        break;
      }
      break;
    case F4:
      switch (type) {
      case R_PRIMITIVE:
      case W_PRIMITIVE:
        // For RW this should be the Y location
        if (strlen(tokbuf)!=1) {
          command_complete=FAILED;
        }
        else {
          if ((strncasecmp(tokbuf,GRID_NUM_MIN,sizeof(GRID_NUM_MIN))>=0) && \
              (strncasecmp(tokbuf,GRID_NUM_MAX,sizeof(GRID_NUM_MIN))<=0)) {
            system_state->instruction.pin_y=(unsigned char) atoi(tokbuf);
            field=F5;
          }
          else {
            command_complete=FAILED;
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
          break;
        }
        else {
          *strptr=0; // since newline found, strip it from tokbuf for V/I match
        }
        if (strlen(tokbuf)==0) {
          command_complete=FAILED; // empty value field is an error
        }
        else {
          value_temp=atoi(tokbuf);
          if ((value_temp>W_VALUE_MAX)||(value_temp<W_VALUE_MIN)) {
            command_complete=FAILED; // empty value field is an error
          }
          else {
            system_state->instruction.value=(unsigned short) value_temp;
            command_complete=DONE;
          }
        }
        break;
      }
      break;
    case F6:
      if (!strncasecmp(tokbuf,TERMINATOR,sizeof(TERMINATOR))) {
        printf("Found Terminator\n");
      }
      else {
        printf("F6 %s\n",tokbuf);
      }
      field=F7;
      break;
    case F7:
      if (!strncasecmp(tokbuf,TERMINATOR,sizeof(TERMINATOR))) {
        printf("Found Terminator\n");
      }
      else {
        printf("F7 %s\n",tokbuf);
      }
      field=F8;
      break;
    case F8:
      if (!strncasecmp(tokbuf,TERMINATOR,sizeof(TERMINATOR))) {
        printf("Found Terminator\n");
      }
      else {
        printf("F8 %s\n",tokbuf);
      }
      field=F9;
      break;
    case F9:
      if (!strncasecmp(tokbuf,TERMINATOR,sizeof(TERMINATOR))) {
        printf("Found Terminator\n");
      }
      else {
        printf("F9 %s\n",tokbuf);
      }
      field=F10;
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
    printf("OK\n");
  }
  else {
    printf("NOK\n");
  }
  return error_code;
}

/* interpreter.c ends here */

