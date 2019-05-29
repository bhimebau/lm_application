/* lla.c --- 
 * 
 * Filename: lla.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Fri Mar 31 08:56:37 2006
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

#ifndef LINUX 
#include "ueaclib.h"
#endif 

#include "ueac.h"
#include "lla.h"
#include "conversion.h"

#define INSTR ueac_state->instruction
#define LLA_TABLE ueac_state->lla_table

#define DISABLE 0
#define ENABLE 1
#define UNINITIALIZED 0xFF

int lla_report (ueac_t *ueac_state,lla_data_t *lla_data) {
  int index;
  int ret_val=-1;
  index=INSTR.lla_descriptor-1;
  if (LLA_TABLE[index].enable==ENABLE) {
    lla_data->input=LLA_TABLE[index].in_val;
    lla_data->output=LLA_TABLE[index].out_val;
    ret_val=0;
  }
  return ret_val;
}

int lla_add (ueac_t *ueac_state) {
  int index;
  index=INSTR.lla_descriptor-1;
  LLA_TABLE[index].enable=DISABLE;
  LLA_TABLE[index].in_pin=(INSTR.pin_y-1)*5+(INSTR.pin_x-1);
  if ((INSTR.pin_x_alt==0)||(INSTR.pin_y_alt==0)) {
    LLA_TABLE[index].out_pin=0xFF;
  }
  else {
    LLA_TABLE[index].out_pin=(INSTR.pin_y_alt-1)*5+(INSTR.pin_x_alt-1);
  }
  LLA_TABLE[index].function_ptr=(int *)lla_function_index[INSTR.lla_function-1];
  LLA_TABLE[index].period=INSTR.lla_period;
  LLA_TABLE[index].accumulator=0;
  ueac_state->lla_input|=(unsigned long)(0x00000001<<LLA_TABLE[index].in_pin);
#ifndef LINUX 
  write_lla(LLA_TABLE[index].in_pin,1);
#endif 
  LLA_TABLE[index].in_val=0;
  LLA_TABLE[index].out_val=0;
  LLA_TABLE[index].enable=ENABLE;
  return 0;
}

int lla_disable (ueac_t *ueac_state) {
  int index;
  int ret_val;
  index=INSTR.lla_descriptor-1;
  if (LLA_TABLE[index].enable==ENABLE) {
    ueac_state->lla_input&=~((unsigned long)(0x00000001<<LLA_TABLE[index].in_pin));
    LLA_TABLE[index].enable=DISABLE;
#ifndef LINUX    
    write_lla(LLA_TABLE[index].in_pin,0);
    write_current(LLA_TABLE[index].out_pin,0);
#endif 
    ret_val=0;
  }
  else {
    ret_val=-1;
  }
  return(ret_val);
}

int lla_enable (ueac_t *ueac_state) {
  int index;
  int ret_val;
  index=INSTR.lla_descriptor-1;
  if (LLA_TABLE[index].enable==DISABLE) {
    ueac_state->lla_input|=(unsigned long)(0x00000001<<LLA_TABLE[index].in_pin);
#ifndef LINUX
    write_lla(LLA_TABLE[index].in_pin,1);
    write_current(LLA_TABLE[index].out_pin,0);
#endif 
    LLA_TABLE[index].in_val=0;
    LLA_TABLE[index].out_val=0;
    LLA_TABLE[index].enable=ENABLE;
    ret_val=0;
  }
  else {
    ret_val=-1;
  }
  return(ret_val);
}

int evaluate_lla(ueac_t *ueac_state) {
  int ret_val;
  ueacval_t result;
  static unsigned char count=0;
  if (++count>9) {
    count=0;
  }
  if (LLA_TABLE[count].enable==ENABLE) {
#ifndef LINUX
    convert_a2d(I_CONVERSION,pin_data[LLA_TABLE[count].in_pin].filtered_result,&result);
#else 
    convert_a2d(I_CONVERSION,0,&result);
#endif
    LLA_TABLE[count].in_val=result.integer;      
    if (++LLA_TABLE[count].counter>=LLA_TABLE[count].period) {
      LLA_TABLE[count].counter=0;
      LLA_TABLE[count].accumulator+=LLA_TABLE[count].in_val;
      LLA_TABLE[count].in_val=(int)(LLA_TABLE[count].accumulator/LLA_TABLE[count].period);
      if (LLA_TABLE[count].in_val>200) {
        LLA_TABLE[count].out_val=*(LLA_TABLE[count].function_ptr+200);
      }
      else {
        LLA_TABLE[count].out_val=*(LLA_TABLE[count].function_ptr+LLA_TABLE[count].in_val);
      }
      if (LLA_TABLE[count].out_pin!=0xFF) {
#ifndef LINUX
        write_current(LLA_TABLE[count].out_pin,LLA_TABLE[count].out_val);
#endif
      }
      LLA_TABLE[count].accumulator=0;
    }
    else {
      LLA_TABLE[count].accumulator+=LLA_TABLE[count].in_val;
    }
  }
  return (ret_val);
}

void lla_print_active (ueac_t *ueac_state) {
  int i;
  printf ("lla_input var=0x%08lx\n",ueac_state->lla_input);
  for (i=0;i<10;i++) {
#ifdef LINUX
    printf("\nLLA %d\n",i+1);
    if (LLA_TABLE[i].enable==ENABLE) {
      printf("enable state=ENABLED\n");
      printf("input pin = %d\n",LLA_TABLE[i].in_pin);
      printf("output pin = %d\n",LLA_TABLE[i].out_pin);
      printf("first data val = %d\n",*(LLA_TABLE[i].function_ptr+100));
      printf("period = %d\n",LLA_TABLE[i].period);
      printf("in_val = %d\n",LLA_TABLE[i].in_val);
      printf("out_val = %d\n",LLA_TABLE[i].out_val);
      printf("accumulator = %ld\n",LLA_TABLE[i].accumulator);
    }
    else {
      printf("enable state=DISABLED\n");
    }
#elif 
    printf("\nLLA %d\n\r",i+1);
    if (LLA_TABLE[i].enable==ENABLE) {
      printf("enable state=ENABLED\n\r");
      printf("input pin = %d\n\r",LLA_TABLE[i].in_pin);
      printf("output pin = %d\n\r",LLA_TABLE[i].out_pin);
      printf("first data val = %d\n\r",*(LLA_TABLE[i].function_ptr+100));
      printf("period = %d\n\r",LLA_TABLE[i].period);
      printf("in_val = %d\n\r",LLA_TABLE[i].in_val);
      printf("out_val = %d\n\r",LLA_TABLE[i].out_val);
      printf("accumulator = %ld\n\r",LLA_TABLE[i].accumulator);
    }
    else {
      printf("enable state=DISABLED\n");
    }
#endif

  }
}



/* lla.c ends here */
