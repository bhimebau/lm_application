/* ueaclib.h --- 
 * 
 * Filename: ueaclib.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Wed Nov  2 11:25:08 2005
 * Version: 
 * Last-Updated: Tue Nov  8 11:46:35 2005
 *           By: Bryce Himebaugh
 *     Update #: 3
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
void init_sys(void);
int putchar_0(int);
char getchar_0(void);
int putchar(int);
char getchar(void);
unsigned char send_spi_byte(unsigned char);
void write_dac(int,int);
void write_current(int,int);
void write_led (int,int);
void write_lla (int,int);

void start_a2d_converter(void);
int wait_a2d_busy(void);
void write_analog_mux(unsigned char);
void system_reset(void);
void led_pwm(int);
void clear_led_screen(void);


#define BLACK "\x1B[30m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define BACK_WHITE "\x1B[47m"

#define CLR "\033[2J"        // VT100 escape sequence to clear the terminal
#define TERM_RESET "\033[H"  // VT100 escape sequence to reset the cursor

/* ueaclib.h ends here */
