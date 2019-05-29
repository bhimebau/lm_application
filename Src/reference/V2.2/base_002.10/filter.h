/* filter.h --- 
 * 
 * Filename: a2d_filter.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Fri Mar 24 09:07:49 2006
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

#define FILTER_DEPTH 16

typedef struct channel {
  unsigned char insertion_point;
  unsigned short channel_data[FILTER_DEPTH];
  unsigned short raw_result;
  unsigned short filtered_result;
} channel_t;

int initialize_channel_structure(channel_t *);
unsigned short update_a2d_data (channel_t *,unsigned short);
int report_channel_structure(channel_t *);

/* filter.h ends here */
