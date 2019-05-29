/* lla.h --- 
 * 
 * Filename: lla.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Fri Mar 31 08:56:44 2006
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

#define LLA_DESCRIPTOR_MAX 10

typedef struct lla_data {
  int input;
  int output;
} lla_data_t;

int lla_report (ueac_t *,lla_data_t *);
int lla_add (ueac_t *);
int lla_disable (ueac_t *);
int lla_enable (ueac_t *);
int evaluate_lla(ueac_t *);
void lla_print_active(ueac_t *);

extern const int *lla_function_index[];

/* lla.h ends here */
