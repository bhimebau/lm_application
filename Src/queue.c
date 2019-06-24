/* queue.c --- 
 * 
 * Filename: queue.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Thu Feb  7 19:49:26 2013 (-0500)
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

#include "queue.h"
#include "interrupt.h"

void init_queue(queue_t *buf) {
  uint32_t mask;
  mask = disable();
  buf->head=0;
  buf->tail=0;
  restore(mask);
}

int enqueue (queue_t *buf, uint8_t data) {
  int return_val=0;
  uint32_t mask;
  mask = disable();
  if ((((buf->head)+1)%QUEUE_SIZE)!=buf->tail) {
    buf->buffer[buf->head]=data;   
    buf->head=((buf->head)+1)%QUEUE_SIZE;
    return_val=0;
  }
  else {
    return_val=-1;
  }
  restore(mask);
  return (return_val);
}

uint8_t dequeue (queue_t *buf) {
  int return_val=0;
  uint32_t mask;
  mask = disable();
  if (buf->tail!=buf->head) {
    return_val=buf->buffer[buf->tail];
    buf->tail=((buf->tail)+1)%QUEUE_SIZE;
  }
  restore(mask);
  return(return_val);
}

int queue_empty(queue_t *buf) {
  int retval = 0;
  uint32_t mask;
  mask = disable();
  if (buf->head==buf->tail) {
    retval=1;
  }
  restore(mask);
  return (retval);
}


/* queue.c ends here */
