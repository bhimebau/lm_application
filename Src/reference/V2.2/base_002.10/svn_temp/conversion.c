/* conversion.c --- 
 * 
 * Filename: conversion.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Mon Mar 27 10:00:47 2006
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
#include "conversion.h"
#include "vi_table.h"
#include "ueac.h"
#include "filter.h"
#include "global.h"

// dig_a2d - 12-bit digital value returned from a2d
// v_a2d - analog voltage at a2d pin
// v_a2d = (3.3/4096) * dig_a2d
// v_op = (v_a2d*2215)/715
// (lla mode off) v_pin = v_op 
// (lla mode on) v_pin = v_op/11
// v_pin = (((3.3/4096)*dig_a2d)*2215)/715 
// i_pin = (v_pin*1000)/11 (lookup table) 

void convert_a2d(char value_type,unsigned short a2d_data,ueacval_t *return_val,int pin) {
  if (value_type==V_CONVERSION) {
    if (a2d_data < 400) {
      return_val->integer=0;
      return_val->hundredth=a2d_data>>2;
    }
    else if (a2d_data < 800) {
      return_val->integer=1;
      return_val->hundredth=(a2d_data-400)>>2;
    }
    else if (a2d_data < 1200) {
      return_val->integer=2;
      return_val->hundredth=(a2d_data-800)>>2;
    }
    else if (a2d_data < 1600) {
      return_val->integer=3;
      return_val->hundredth=(a2d_data-1200)>>2;
    }
    else if (a2d_data < 2000) {
      return_val->integer=4;
      return_val->hundredth=(a2d_data-1600)>>2;
    }
    else if (a2d_data < 2400) {
      return_val->integer=5;
      return_val->hundredth=(a2d_data-2000)>>2;
    }
    else if (a2d_data < 2800) {
      return_val->integer=6;
      return_val->hundredth=(a2d_data-2400)>>2;
    }
    else if (a2d_data < 3200) {
      return_val->integer=7;
      return_val->hundredth=(a2d_data-2800)>>2;
    }
    else if (a2d_data < 3600) {
      return_val->integer=8;
      return_val->hundredth=(a2d_data-3200)>>2;
    }
    else if (a2d_data < 4000) {
      return_val->integer=9;
      return_val->hundredth=(a2d_data-3600)>>2;
    }
    else {
      return_val->integer=10;
      return_val->hundredth=(a2d_data-4000)>>2;
    }
  }
  else {
    return_val->integer=a2d_i_conversion[a2d_data];
    if (pin<25) {
      if (return_val->integer<=ueac_state.pin_cal[pin].i_in_floor) {
        return_val->integer=0;
      }
      else {
        return_val->integer+=ueac_state.pin_cal[pin].i_in_offset;
      }
    }
    return_val->hundredth=0;
  }
}
/* conversion.c ends here */
