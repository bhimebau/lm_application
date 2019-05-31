/* ueac.h --- 
 * 
 * Filename: ueac.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Tue Feb 28 12:28:18 2006
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

// Grid values 
#define VOLTAGE_SENSE 0x1000
#define I_CONSTANT 0x2000
#define LLA_IN 0x4000
#define LLA_OUT 0x8000

// ueac modes
#define UEAC_HALT 0x01
#define UEAC_RUN 0x02

// ueac_instruction commands 
#define UEAC_READY  0x00 
#define UEAC_EXECUTE 0x01

// ueac_instruction commands 
#define UEAC_ALL_V 0
#define UEAC_ALL_I 1 
#define UEAC_READ_V 2
#define UEAC_READ_I 3
#define UEAC_WRITE 4
#define UEAC_LLA_ADD 5
#define UEAC_LLA_DISABLE 6
#define UEAC_LLA_ENABLE 7e
#define UEAC_LLA_REPORT 8
#define UEAC_RST 9
#define UEAC_CAL 10
#define UEAC_LON 11
#define UEAC_LOF 12
#define UEAC_DEBUG 13

typedef struct ueac_instruction {
  unsigned char command_reg;           // command that starts the instruction processing
  unsigned char instruction_type;      // tells command execution phase which instruction to execute
  unsigned char lla_descriptor;        // indicates the particular lla to use  
  unsigned char pin_x;                 // X,Y location of the probe site to be used, LLA input pins
  unsigned char pin_y;
  unsigned char pin_x_alt;             // X,Y location of the probe site to be used, LLA output pins
  unsigned char pin_y_alt;
  int value;                           // value to be applied when in write mode.
  unsigned char lla_function;    
  unsigned char lla_period;
} ueac_instruction_t;

typedef struct lla {
  unsigned char enable;                // 0=lla disabled, 1=lla enabled, 0xFF unitialized. 
  unsigned char in_pin;                // 0 to 24 
  unsigned char out_pin;               // 0 to 24 + 0xFF (if all FF then no sheet output). 
  unsigned int *function_ptr;          // pointer to the base of the lla lookup table
  unsigned char period;                // write 1-255 
  unsigned char counter;
  int in_val;
  int out_val;
  long accumulator; 
} lla_t;

typedef struct cal {
  char i_200uA_offset;
  char i_zero_offset;
  char i_in_floor;
  char i_in_offset;
} cal_t;

typedef struct ueac {
  ueac_instruction_t instruction;      // request to modify the state of the system
  unsigned long lla_input;             // bit packed long where a 1 indicates that is an lla input. 
  unsigned short pin_current[25];      // current flowing to/from a particular pin driver. 
  cal_t pin_cal[25];
  lla_t lla_table[10];                 // structure to track the lla pin relationships
} ueac_t;  

// Function Prototypes 
extern int init_ueac(ueac_t *);
extern int display_ueac(ueac_t *);
extern int ueac_execute_instruction(ueac_t *);
extern int lla_input_check(char,ueac_t *);

/* ueac.h ends here */
