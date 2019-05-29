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

typedef struct ueac_instruction {
  unsigned char command_reg;           // command that starts the instruction processing
  unsigned char instruction_type;      // tells command execution phase which instruction to execute
  unsigned char pin_x;                 // X,Y location of the probe site to be used
  unsigned char pin_y;
  unsigned short value;                // value to be applied when in write mode. 
} ueac_instruction_t;



typedef struct lla {
  char *tag;   
  unsigned char in_pin;                // 0 to 24 
  unsigned char out_pin;               // 0 to 24 + 0xFF (if all FF then no sheet output). 
  unsigned char function_num;          // 0 to 52 to represent a look-up table
  unsigned char rate;                  // writing a zero here disables LLA evaluation, else write 1-255 to get rate reduction
} lla_t;

typedef struct ueac {
  unsigned short mode;
  ueac_instruction_t instruction;      // request to modify the state of the system
  unsigned long pin_output_enable;     // bit packed output enable for each pin in the grid
  unsigned short grid[25];             // upper 4 bits define the pin function, lower 12 bits are data  
                                       // VOLTAGE_SENSE - raw A2D value (grid voltage) 
                                       // I_CONSTANT - raw DAC value for V-I converter
                                       // LLA_IN - raw A2D value for current sense (amplified version of V sense)
                                       // LLA_OUT - raw A2D value (grid voltage) 
  unsigned short lla_output_i[25];     // raw DAC value indicating the current flowing out of the LLA output
  lla_t lla_table[25];                 // structure to track the lla pin relationships
  unsigned char lla_eval_schedule[25];
} ueac_t;  

// Function Prototypes 
extern int init_ueac(ueac_t *);
extern int display_ueac(ueac_t *);
extern int ueac_execute_instruction(ueac_t *);

/* ueac.h ends here */
