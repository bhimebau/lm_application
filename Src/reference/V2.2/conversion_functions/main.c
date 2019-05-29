/* main.c --- 
 * 
 * Filename: main.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Tue Feb 28 08:55:30 2006
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
#include <stdlib.h>
#include "conversion.h"


ueacval_t conversion_result;

int main(void) {
  convert_a2d(V_CONVERSION,1830,&conversion_result);
  printf("Voltage = %d.%d\n",conversion_result.integer,conversion_result.hundredth);

  convert_a2d(I_CONVERSION,0x370,&conversion_result);
  printf("Current = %d Voltage = 0.%d\n",conversion_result.integer,conversion_result.integer);

  printf("Working Base\n");
  printf("done\n");
  return 0; 
}






/* main.c ends here */
