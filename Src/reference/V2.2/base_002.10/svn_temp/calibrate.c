/* calibrate.c --- 
 * 
 * Filename: calibrate.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Fri Feb 10 09:48:40 2006
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

#include <msp430x16x.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ueaclib.h"
#include "ueac.h"
#include "calibrate.h"
#include "conversion.h"
#include "timer.h"
#include "filter.h"
#include "global.h"
#include "external_flash.h"

extern short dac_translation[];
#define LED_DWELL 100

/* Code: */
void scan_leds(void) {
  enum {OFF,ON};
  int index; 
  printf("Scanning LEDs ");
  // All LEDs off
  for (index=0;index<25;index++) {
    write_led(index,OFF);
  }
  // Scan Rows 
  for (index=0;index<25;index+=5) {
    write_led(index,ON);
    write_led(index+1,ON);
    write_led(index+2,ON);
    write_led(index+3,ON);
    write_led(index+4,ON);
    delay_1_25mS(LED_DWELL);
    write_led(index,OFF);
    write_led(index+1,OFF);
    write_led(index+2,OFF);
    write_led(index+3,OFF);
    write_led(index+4,OFF);
  }
  // Scan Columns 
  for (index=0;index<5;index++) {
    write_led(index,ON);
    write_led(index+5,ON);
    write_led(index+10,ON);
    write_led(index+15,ON);
    write_led(index+20,ON);
    delay_1_25mS(LED_DWELL);
    write_led(index,OFF);
    write_led(index+5,OFF);
    write_led(index+10,OFF);
    write_led(index+15,OFF);
    write_led(index+20,OFF);
  }
  // All LEDs ON
  for (index=0;index<25;index++) {
    write_led(index,ON);
  }
  delay_1_25mS(LED_DWELL*2);
  // All LEDs OFF
  for (index=0;index<25;index++) {
    write_led(index,OFF);
  }
  printf(GREEN);
  printf("[DONE]\n\r");
  printf(BLACK);
}

void current_output_calibration (ueac_t *system) {
  enum {FIND_ESCAPE,FIND_LEFT_BRACKET,EVALUATE_ARROW};
  int i;
  char ch;
  int parse_state=FIND_ESCAPE;
  int i_offset=0;
  int found=0;
  int sum,current;
  ueacval_t result;


  printf("\n\rCurrent Output Calibration\n\r");
  printf("Attach the DVM in current mode to the pin with the active LED\n\r");
  printf("Use the up arrow and down arrow keys to adjust the current\n\r");
  printf("-----------------------------------------------------------------\n\r");
  buffer1_write(0,'V');        // write the valid char to the first location of external flash                          
  for (i=1;i<PAGE_SIZE;i++) {  // clear out the external flash's SRAM buffer
    buffer1_write(i,0);
  }
  for (i=0;i<25;i++) {
    write_lla(i,0);
    write_led(i,1);
    found=0;
    i_offset=0;
    printf("%d@200uA> ",i);
    while (!found) {
      write_dac(i,dac_translation[0]+i_offset);  
      if (((ch=getchar())!='\n')&&(ch!='\r')) {
        switch (parse_state) {
        case FIND_ESCAPE:
          if (ch==0x1b) {
            parse_state=FIND_LEFT_BRACKET;
          }
          break;
        case FIND_LEFT_BRACKET:
          if (ch=='[') {
            parse_state=EVALUATE_ARROW;
          }
          else {
            parse_state=FIND_ESCAPE;
          }
          break;
        case EVALUATE_ARROW: 
          if (ch=='A') {
            i_offset--;
          }
          else if (ch=='B') {
            i_offset++;
          }
          parse_state=FIND_ESCAPE;
          break;
        }
      }
      else {
        buffer1_write(i+1,i_offset);
        printf("%d\n\r",i_offset);
        found=1;
        system->pin_cal[i].i_200uA_offset=i_offset;
      }
    }
    found=0;
    i_offset=0;
    printf("%d@0uA> ",i);
    while (!found) {
      write_dac(i,dac_translation[200]+i_offset);  
      if (((ch=getchar())!='\n')&&(ch!='\r')) {
        switch (parse_state) {
        case FIND_ESCAPE:
          if (ch==0x1b) {
            parse_state=FIND_LEFT_BRACKET;
          }
          break;
        case FIND_LEFT_BRACKET:
          if (ch=='[') {
            parse_state=EVALUATE_ARROW;
          }
          else {
            parse_state=FIND_ESCAPE;
          }
          break;
        case EVALUATE_ARROW: 
          if (ch=='A') {
            i_offset--;
          }
          else if (ch=='B') {
            i_offset++;
          }
          parse_state=FIND_ESCAPE;
          break;
        }
      }
      else {
        buffer1_write(i+26,i_offset);
        printf("%d %d\n\r",i_offset,dac_translation[200]+i_offset);
        found=1;
        system->pin_cal[i].i_zero_offset=i_offset;
      }
    }
    write_led(i,0);
  }
  for (i=0;i<25;i++) {
    write_lla(i,1);
    printf("%d ",i);
    sum=0;
    for (current=200;current>0;current-=50) {
      write_current(i,current);
      delay_1_25mS(500);
      convert_a2d(I_CONVERSION,pin_data[i].filtered_result,&result,25);
      printf("%d ",result.integer);
      sum+=(current-result.integer);
    }
    write_current(i,0);
    delay_1_25mS(500);
    convert_a2d(I_CONVERSION,pin_data[i].filtered_result,&result,25);
    printf("floor=%d,",result.integer);
    buffer1_write(i+51,result.integer);
    printf("offset=%d\n\r",sum/4);
    buffer1_write(i+76,sum/4);
    write_lla(i,0);
  }
  printf("Commit calibration to external flash ? (y or n) ");
  while (1) {
    if (((ch=getchar())=='y')||(ch=='Y')) {
      buffer1_to_page_e(0);
      printf("\n\rCalibration Saved !\n\r");
      break;
    }
    if ((ch=='n')||(ch=='N')) {
      printf("\n\rCalibration Abort !\n\r");
      break;
    }
    else {
      printf("\n\rPlease respond with y or n: ");
    }
  }
}

void current_input_calibration (ueac_t *system) {
  printf("\n\rCurrent Input Calibration\n\r");
}

void voltage_input_calibration (ueac_t *system) {
  printf("\n\rVoltage Input  Calibration\n\r");
}




/* calibrate.c ends here */
