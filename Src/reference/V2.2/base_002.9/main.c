/* main.c --- 
 * 
 * Filename: main.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Wed Nov  2 11:19:17 2005
 * Version: 
 * Last-Updated: Tue Nov  8 16:50:17 2005
 *           By: Bryce Himebaugh
 *     Update #: 236
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

/* Doxygen Main index docs */
/*! \mainpage uEAC Hybrid Computer Operating System  
    \authors Bryce Himebaugh
    \section A uEAC Command Interpreter Documentation
    <ul>
        <li> <a href="../interpreter_doc/index.html"> Command Language </a>
    </ul> 
    \section B uEAC R002 Schematics 
    <ul> 
        <li> <a href="../pdf/sheet1.pdf"> Top Level </a>
        <li> <a href="../pdf/sheet2.pdf"> Probe Row 0</a>
        <li> <a href="../pdf/sheet3.pdf"> Probe Row 1</a>
        <li> <a href="../pdf/sheet4.pdf"> Probe Row 2</a>
        <li> <a href="../pdf/sheet5.pdf"> Probe Row 3</a>
        <li> <a href="../pdf/sheet6.pdf"> Probe Row 4</a>
        <li> <a href="../pdf/sheet7.pdf"> FTDI Serial/Power Supplies</a>
    </ul> 
    \section C Component Reference Docs 
    <ul> 
        <li> <a href="../pdf/slau049e-1.pdf"> TI MSP430 User's Manual</a>
        <li> <a href="../pdf/ds2232c_15.pdf"> FTDI DS2232C Datasheet</a>
    </ul>    
  */ 

/* Code: */
#include <msp430x16x.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ueaclib.h"
#include "ueac.h"
#include "external_flash.h"
#include "interpreter.h"
#include "filter.h"
#include "conversion.h"
#include "global.h"
#include "timer.h"
#include "calibrate.h"

void print_grid_v();
void print_grid_i();
int get_command(char *command_buf);
extern short dac_translation[];

int main(void) {
  int i;
  init_sys();                        // setup the MSP430 peripherals
  printf(BACK_WHITE);
  printf(CLR);
  printf("UEAC POST\n\r");
  flash_integrity_test();
  scan_leds();
  init_global_variables();
  for (i=0;i<25;i++) {
    write_lla(i,0);
    write_led(i,0);
    write_current(i,0);
  }
  delay_1_25mS(800);     
  printf(CLR);
  printf("uEACos V2.2 base_002.9\n\r");
  led_screen_enable=1;
  while(1) {
    printf("ueac> ");
    get_command(command);
    interpreter(command,&ueac_state);
  }
}

void scan_probes() {
  int i,j;
  for (i=0;i<25;i++) {
    write_lla(i,1);
    printf("%d ",i);
    for (j=200;j>=0;j-=50) {
      write_current(i,j);
      delay_1_25mS(400);     
      convert_a2d(I_CONVERSION,pin_data[i].filtered_result,&conversion_result,i);
      printf("%d ",conversion_result.integer);
    }
    printf("\n\r");
    write_lla(i,0);
  }
}

int get_command(char *command_buf) {
  char ch;
  int counter=0;
  while(((ch=getchar())!='\n')&&(ch!='\r')&&(counter++<100)) {
    *command_buf++=ch;
  }
  *command_buf++='\n';
  *command_buf=0;
  return (counter);
}     

void print_grid_v() {
  int i,j;
  for (i=0;i<5;i++) {
    for (j=0;j<5;j++) {
      convert_a2d(V_CONVERSION,pin_data[i*5+j].filtered_result,&conversion_result,i*5+j); 
      printf("%d.%d ",conversion_result.integer,conversion_result.hundredth);
    }
    printf("\n\r");
  }
}

void print_grid_i() {
  int i,j;
  for (i=0;i<5;i++) {
    for (j=0;j<5;j++) {
      write_lla(i*5+j,1);
      write_current(i*5+j,200);
      delay_1_25mS(500);
      convert_a2d(I_CONVERSION,pin_data[i*5+j].filtered_result,&conversion_result,i*5+j); 
      printf("%d ",conversion_result.integer);
      write_lla(i*5+j,0);
    }
    printf("\n\r");
  }
}



/* main.c ends here */
