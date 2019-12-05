
/* queue.h --- 
 * 
 * Filename: queue.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Thu Feb  7 19:47:41 2013 (-0500)
 * Version: 
 * Last-Updated: 
 *           By: 
 *     Update #: 0
 * URL: 
 * Doc URL: 
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change Log:
 * 
 * 
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */
#include "stm32l4xx.h"

#define PQUEUE_SIZE 32

#ifndef PQUEUE_H
#define PQUEUE_H

typedef struct pqueue {
  int head;
  int tail;
  uint8_t buffer[PQUEUE_SIZE];
} pqueue_t;

#endif


void init_pqueue(pqueue_t *);
int penqueue(pqueue_t *, uint8_t );
uint8_t pdequeue(pqueue_t *);            
int pqueue_empty(pqueue_t *);            

/* queue.h ends here */
