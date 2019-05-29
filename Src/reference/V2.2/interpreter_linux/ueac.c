/* ueac.c --- 
 * 
 * Filename: ueac.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Tue Feb 28 14:47:28 2006
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
#include <stdio.h>
#include "ueac.h"

char unused_tag[]="empty";

int init_ueac(ueac_t *ueac_state) {
  int i;
  ueac_state->mode=UEAC_HALT;
  ueac_state->pin_output_enable=0x00000000;     // all outputs disabled
  for (i=0;i<25;i++) {
    ueac_state->grid[i]=VOLTAGE_SENSE;          // Define all of the pins as voltage sense
    ueac_state->lla_output_i[i]=0x0000;         // Clear the structure for the LLA output  
    ueac_state->lla_eval_schedule[i]=0xFF;      // Write the schedule queue to all inactive
    ueac_state->lla_table[i].tag=unused_tag;           
    ueac_state->lla_table[i].in_pin=0xFF;    
    ueac_state->lla_table[i].out_pin=0xFF;
    ueac_state->lla_table[i].function_num=0xFF;
    ueac_state->lla_table[i].rate=0xFF;
  }

/*   ueac_state->grid[2]=LLA_IN; */
/*   ueac_state->grid[5]=LLA_OUT; */
/*   ueac_state->lla_table[6].in_pin=0x02;     */
/*   ueac_state->lla_table[6].out_pin=0x05;     */

  return 0;
}

int display_ueac(ueac_t *ueac_state) {
  int i,j,k;
  unsigned char found,index;
  
  printf("\n####  UEAC SYSTEM  ####\n");
  switch (ueac_state->mode) {
  case UEAC_HALT:
    printf("STATE = UEAC_HALT\n");
    break;
  case UEAC_RUN:
    printf("STATE = UEAC_RUN\n");
    break;
  default:
    printf("STATE = UNKNOWN\n");
  }

  printf("\n#### GRID ASSIGNMENT ####\n");
  for(i=0;i<5;i++){
    for(j=0;j<5;j++){
      index=5*i+j;
      switch (ueac_state->grid[index]&0xF000) {
      case VOLTAGE_SENSE:
        printf("V        ");
        break;
      case I_CONSTANT:
        printf("I        ");
        break;
      case LLA_IN:
        found=0;
        for (k=0;k<25;k++) {
          if (index==ueac_state->lla_table[k].in_pin) {
            found=1;
            break;
          }
        }
        if (found) {
          printf("LLAI%02d   ",k);
        }
        else {
          printf("LLAI     ");
        }
        break;
      case LLA_OUT:
        found=0;
        for (k=0;k<25;k++) {
          if (index==ueac_state->lla_table[k].out_pin) {
            found=1;
            break;
          }
        }
        if (found) {
          printf("LLAO%02d   ",k);
        }
        else {
          printf("LLAO     ");
        }
        break;
      }
    }
    printf("\n\n");
  }
  return 0;
}

// returns -1 if it could not be added, else it returns the lla slot number that it was stored into 
int add_lla(ueac_t *ueac_state,char *tag,int xin,int yin,int xout,int yout,int function,int speed) {
  return 0;
}

// return 0 if success, -1 if failed
int delete_lla(ueac_t *ueac_state,char *tag,int xin,int yin,int xout,int yout,int function,int speed) {
  return 1;
}

int ueac_execute_instruction(ueac_t *system_state) {
  int return_val=0;
  if (system_state->instruction.command_reg==UEAC_EXECUTE) {
    switch (system_state->instruction.instruction_type) {
    case UEAC_ALL_V:
      printf("UEAC_ALL_V ");
      break;
    case UEAC_ALL_I:
      printf("UEAC_ALL_I ");
      break;
    case UEAC_READ_V:
      printf("UEAC_READ_V ");
      break;
    case UEAC_READ_I:
      printf("UEAC_READ_I ");
      break;
    case UEAC_WRITE:
      printf("UEAC_WRITE ");
      break;
    }
    printf("%d %d %d ",system_state->instruction.pin_x,\
           system_state->instruction.pin_y,\
           system_state->instruction.value);
  }
  else {
    printf("Execute Bit Not Set\n");
    return_val=1;
  }
  system_state->instruction.command_reg=UEAC_READY;
  return(return_val);
}

/* ueac.c ends here */
