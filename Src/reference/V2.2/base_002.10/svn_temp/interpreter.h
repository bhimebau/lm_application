/* interpreter.h --- 
 * 
 * Filename: interpreter.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Tue Feb 28 10:09:23 2006
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

extern int interpreter(char *,ueac_t *);
extern int delspace(char *,char *);

#define W_VALUE_MAX 200        // Current Source Maximum Value in uA
#define W_VALUE_MIN -200       // Current Source Minimum Value in uA               

/* interpreter.h ends here */
