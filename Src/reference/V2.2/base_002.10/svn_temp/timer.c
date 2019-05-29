/* timer.c --- 
 * 
 * Filename: timer.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Fri Mar 24 15:38:38 2006
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
#include <msp430x16x.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ueaclib.h"
#include "ueac.h"
#include "conversion.h"
#include "filter.h"
#include "global.h"
// #include "lla.h"

/*! Timer A1 Interupt Handler
 *
 * Timer A is used as the time base for the scheduler. This interrupt occurs every 1.25mS. 
 */

#include "lla.h"

#ifndef DOX_SKIP 
interrupt (TIMERA0_VECTOR) 
#endif 
timer_a0_irq() {
  static int chan_last=0;
  static int chan_next=0;
  static char state=0;
  TACCR0+=0x900;               // Increment the compare register for 2305 ticks into future (625uS)
  if (state>9) {                // evaluate the system every 1.25mS 
    state=0;
    chan_last=chan_next&0x07;
    write_analog_mux(++chan_next);
    update_a2d_data(&pin_data[chan_last],ADC12MEM0);
    update_a2d_data(&pin_data[8+chan_last],ADC12MEM1);
    update_a2d_data(&pin_data[16+chan_last],ADC12MEM2);
    if (chan_last==0) {
      update_a2d_data(&pin_data[24],ADC12MEM3);
    }
    if (lla_input_check(chan_last,&ueac_state)) {
      high_time_limit[chan_last]=0;
    }
    else {
      high_time_limit[chan_last]=ADC12MEM0>>8;
    }
    if (lla_input_check(8+chan_last,&ueac_state)) {
      high_time_limit[8+chan_last]=0;
    }
    else {
      high_time_limit[8+chan_last]=ADC12MEM1>>8;
    }
    if (lla_input_check(16+chan_last,&ueac_state)) {
      high_time_limit[16+chan_last]=0;
    }
    else {
      high_time_limit[16+chan_last]=ADC12MEM2>>8;
    }

    if (lla_input_check(24,&ueac_state)) {
      high_time_limit[24]=1;
    }
    else {
      high_time_limit[24]=(ADC12MEM3>>8)+1;
    }
    evaluate_lla(&ueac_state);
    start_a2d_converter();
  }
  timer_tick++;
  state++;
  led_pwm(led_screen_enable);
}

void delay_1_25mS(int count) {
  int tick_counter=0;
  while (tick_counter<count) {
    if (timer_tick>1) {
      tick_counter++;
      timer_tick=0;
    }
  }
}






/* timer.c ends here */
