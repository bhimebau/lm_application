/* global.c --- 
 * 
 * Filename: global.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Mon Mar 27 15:26:08 2006
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
#include "filter.h"
#include "conversion.h"
#include "ueac.h"
#include "external_flash.h"
#include "ueaclib.h"
#include "timer.h"

ueac_t ueac_state;
char command[100];
channel_t pin_data[25];
ueacval_t conversion_result;
volatile int timer_tick;
unsigned char led_screen_enable=0;
unsigned char high_time_limit[25] = {1,1,1,1,1,
                                     1,1,1,1,1,
                                     1,1,1,1,1,
                                     1,1,1,1,1,
                                     1,1,1,1,2};

#define LLA_TABLE ueac_state->lla_table

void init_pin_data_structure(void) {
  int i;
  for (i=0;i<25;i++) {
    initialize_channel_structure(&pin_data[i]);
  }
}

void init_ueac_state_structure(void) {
  int i;
  char ch; 
  ueac_state.lla_input=0;
  for (i=0;i<25;i++) {
    ueac_state.pin_current[i]=0;
    ueac_state.lla_table[i].in_pin=0xFF;
    ueac_state.lla_table[i].out_pin=0xFF;
  }

  // Load calibration data from external flash
  start_main_array_read(0,0); 
  ch=send_spi_byte(0);
  if (ch=='V') {
    // valid calibration, proceed with load
    for (i=0;i<25;i++) {
      ueac_state.pin_cal[i].i_200uA_offset=(char)send_spi_byte(0);
    }
    for (i=0;i<25;i++) {
      ueac_state.pin_cal[i].i_zero_offset=(char)send_spi_byte(0);
    }
    for (i=0;i<25;i++) {
      ueac_state.pin_cal[i].i_in_floor=(char)send_spi_byte(0);
    }
    for (i=0;i<25;i++) {
      ueac_state.pin_cal[i].i_in_offset=(char)send_spi_byte(0);
    }
    end_main_array_read();
    //    for (i=0;i<25;i++) {
    //  printf ("%d %d ",i,ueac_state.pin_cal[i].i_200uA_offset);
    //  printf ("%d ",ueac_state.pin_cal[i].i_zero_offset);
    //  printf ("%d ",ueac_state.pin_cal[i].i_in_floor);
    //  printf ("%d\n\r",ueac_state.pin_cal[i].i_in_offset);
    //    }     
  }
  else {
    // no cal, load defaults 
    for (i=0;i<25;i++) {
      ueac_state.pin_cal[i].i_200uA_offset=0x00;
      ueac_state.pin_cal[i].i_zero_offset=0x00;
      ueac_state.pin_cal[i].i_in_floor=0x00;
      ueac_state.pin_cal[i].i_in_offset=0x00;
    }
  }
}

void init_global_variables (void) {
  init_pin_data_structure();   
  init_ueac_state_structure();
  timer_tick=0;
  led_screen_enable=0;
}



/* global.c ends here */
