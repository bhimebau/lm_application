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

#ifndef LINUX
#include "ueaclib.h"
#include "cal_table.h"
#include "filter.h"
#include "conversion.h"
#include "global.h"
#include "calibrate.h"
#endif 

#include "lla.h"

int lla_input_check(char chan,ueac_t *system_state);
void report_instruction(ueac_t *system_state);

lla_data_t lla_data;

int ueac_execute_instruction(ueac_t *system_state) {
  enum {OFF,ON};
  int return_val=0;

#ifndef LINUX
  int i;
  int probe_num;
#endif 
  if (system_state->instruction.command_reg==UEAC_EXECUTE) {
    switch (system_state->instruction.instruction_type) {
    case UEAC_ALL_V:
#ifndef LINUX
      for (i=0;i<25;i++) {
        if (lla_input_check(i,system_state)) {
          convert_a2d(I_CONVERSION,pin_data[i].filtered_result,&conversion_result,i);
          printf("0.%03d,",conversion_result.integer);
        }
        else {
          convert_a2d(V_CONVERSION,pin_data[i].filtered_result,&conversion_result,i);
          printf("%d.%02d,",conversion_result.integer,conversion_result.hundredth);
        }
      }
#else 
      printf("UEAC_ALL_V ");
#endif
      break;
    case UEAC_ALL_I:
#ifndef LINUX
      for (i=0;i<25;i++) {
        if (lla_input_check(i,system_state)) {
          convert_a2d(I_CONVERSION,pin_data[i].filtered_result,&conversion_result,i);
          printf("-%d,",conversion_result.integer);
        }
        else {
          printf("%d,",system_state->pin_current[i]);
        }
      }
#else 
      printf("UEAC_ALL_I ");
#endif
      break;
    case UEAC_READ_V:
#ifndef LINUX
      probe_num=((system_state->instruction.pin_y-1)*5)+(system_state->instruction.pin_x-1);
      if (lla_input_check(probe_num,system_state)) {
        convert_a2d(I_CONVERSION,pin_data[probe_num].filtered_result,&conversion_result,probe_num);
        printf("0.%03d,",conversion_result.integer);
      }
      else {
        convert_a2d(V_CONVERSION,pin_data[probe_num].filtered_result,&conversion_result,probe_num);
        printf("%d.%02d,",conversion_result.integer,conversion_result.hundredth);
      }
#else 
      printf("UEAC_READ_V ");
#endif
      break;
    case UEAC_READ_I:
#ifndef LINUX
      probe_num=((system_state->instruction.pin_y-1)*5)+(system_state->instruction.pin_x-1);
      if (lla_input_check(probe_num,system_state)) {
        convert_a2d(I_CONVERSION,pin_data[probe_num].filtered_result,&conversion_result,probe_num);
        printf("-%d,",conversion_result.integer);
      }
      else {
        printf("%d,",system_state->pin_current[probe_num]);
      }
#else 
      printf("UEAC_READ_I ");
#endif
      break;
    case UEAC_WRITE:
#ifndef LINUX
      probe_num=((system_state->instruction.pin_y-1)*5)+(system_state->instruction.pin_x-1);
      write_current(probe_num,system_state->instruction.value); 
      system_state->pin_current[probe_num]=system_state->instruction.value;
#else 
      printf("UEAC_WRITE ");
#endif
      break;
    case UEAC_LLA_ADD:
      lla_add(system_state);
#ifndef LINUX
#else 
      printf("UEAC_LLA_ADD ");
#endif
      break;
    case UEAC_LLA_DISABLE:
      lla_disable(system_state);
#ifndef LINUX
#else 
      printf("UEAC_LLA_DISABLE ");
#endif
      break;
    case UEAC_LLA_ENABLE:
      lla_enable(system_state);
#ifndef LINUX
#else 
      printf("UEAC_LLA_ENABLE ");
#endif
      break;
    case UEAC_LLA_REPORT:
      if (lla_report(system_state,&lla_data)==0) {
        printf("-%d,%d,",lla_data.input,lla_data.output);
      }
      else {
        printf("*,*,");
      }
#ifndef LINUX
#else 
      printf("UEAC_LLA_REPORT ");
#endif
      break;
    case UEAC_CAL:
#ifndef LINUX
      current_output_calibration(&ueac_state);
#else 
      printf("UEAC_CAL ");
#endif
      break;
    case UEAC_RST:
#ifndef LINUX
      printf("OK\n\r");
      system_reset();
#else 
      printf("UEAC_RST ");
#endif
      break;
    case UEAC_LON:
#ifndef LINUX
      led_screen_enable=1;
#else 
      printf("UEAC_LON ");
#endif
      break;
    case UEAC_LOF:
#ifndef LINUX
      led_screen_enable=0;
      clear_led_screen();
#else 
      printf("UEAC_LOF ");
#endif
      break;
    }
#ifndef LINUX
    printf("OK\n\r");
#else 
    printf("OK\n");
#endif
  }
  else {
#ifndef LINUX
    printf("NOK\n\r");
#else 
    printf("NOK\n");
#endif 
    return_val=1;
  }
  system_state->instruction.command_reg=UEAC_READY;
  return(return_val);
}

int lla_input_check(char chan,ueac_t *system_state) {
  int return_val=0;
  if (system_state->lla_input&((unsigned long)(0x00000001<<chan))) {
    return_val=1;
  }
  return (return_val);
}

void report_instruction(ueac_t *system_state) {
  
  printf("xin=%d ",system_state->instruction.pin_x);
  printf("yin=%d ",system_state->instruction.pin_y);
  printf("xout=%d ",system_state->instruction.pin_x_alt);
  printf("yout=%d\n",system_state->instruction.pin_y_alt);
  printf("descriptor=%d ",system_state->instruction.lla_descriptor);
  printf("function=%d ",system_state->instruction.lla_function);
  printf("period=%d\n",system_state->instruction.lla_period);
}

/* ueac.c ends here */
