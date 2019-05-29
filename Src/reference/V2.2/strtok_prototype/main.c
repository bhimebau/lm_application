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
#include <string.h>
#include <stdio.h>

int main(void) {
  char in_string[]="a,b,c,d,e";
  char target[]="B";
  const char *delimiter=",";
  char *token = NULL;
  int cmpval;

  token = strtok(in_string,delimiter);
  while (token!=NULL) {
    if (!strncasecmp(token,target,3)) {
      printf("found target ");
    }
    printf("%s\n",token);
    token = strtok(NULL,delimiter);
  }
  return 0;
}




/* main.c ends here */
