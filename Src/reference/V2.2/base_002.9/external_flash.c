/* external_flash.c --- 
 * 
 * Filename: external_flash.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Fri Feb 10 11:13:10 2006
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

#include <msp430x16x.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ueaclib.h"
#include "external_flash.h"

int flash_integrity_test(void) {
  enum {PASS,FAIL};
  int i,fail_flag; 
  unsigned char data=0;
  // unsigned short page=0;
  // printf("### Flash Tests ###\n\r");
  // SRAM BUFFER 1

  //  poll_status_register_blocking();                  // Wait for the flash page erase/write to complete 
  fail_flag=PASS;
  data=0;

  for (i=0;i<PAGE_SIZE;i++) {
    buffer1_write((unsigned short)i,data++);         // SRAM Buffer 1 Fill 
  }
  data=0;
  for (i=0;i<PAGE_SIZE;i++) {
    if (data++!=buffer1_read(i)) {
      fail_flag=FAIL;
    }
  }
  printf("External Flash SRAM Buffer 1 Fill ");
  if (fail_flag==FAIL) {
    printf(RED);
    printf("[FAILED]\n\r");
  }
  else {
    printf(GREEN);
    printf("[PASSED]\n\r");
  }
  printf(BLACK);

  // BUFFER 2
  fail_flag=PASS;
  data=0;
  for (i=0;i<PAGE_SIZE;i++) {
    buffer2_write((unsigned short)i,data++);         // SRAM Buffer 1 Fill 
  }
  data=0;
  for (i=0;i<PAGE_SIZE;i++) {
    if (data++!=buffer2_read(i)) {
      fail_flag=FAIL;
    }
  }
  printf("External Flash SRAM Buffer 2 Fill ");
  if (fail_flag==FAIL) {
    printf(RED);
    printf("[FAILED]\n\r");
  }
  else {
    printf(GREEN);
    printf("[PASSED]\n\r");
  }
  printf(BLACK);
  
  // FLASH PAGES
/*   fail_flag=PASS; */
/*   printf("External Flash Array Write \r"); */
/*   for (page=0;page<NUMBER_PAGES;page++){ */
/*     printf("External Flash Array Write %d\r",page); */
/*     buffer1_to_page_e(page); */
/*     poll_status_register_blocking();                  // Wait for the flash page erase/write to complete */
/*     start_main_array_read(page,0); */
/*     data=0; */
/*     for (i=0;i<PAGE_SIZE;i++) { */
/*       if (data++!=send_spi_byte(0)) { */
/*         fail_flag=FAIL; */
/*       } */
/*     } */
/*     end_main_array_read(); */
/*   } */
/*   printf("External Flash Array Write "); */
/*   if (fail_flag==FAIL) { */
/*     printf(RED); */
/*     printf("[FAILED]\n\r"); */
/*   } */
/*   else { */
/*     printf(GREEN); */
/*     printf("[PASSED]\n\r"); */
/*   } */
/*   printf(BLACK); */
  return (0);
}

void start_main_array_read(unsigned short page,unsigned short addr) {
  unsigned char page_temp; 

  nCS_LOW;
  // opcode
  send_spi_byte(0xE8);  // SPI Mode 0 or 3  

  // Prepare/send the page and address fields
  page_temp = (unsigned char)(page>>7);
  send_spi_byte (page_temp);
  page_temp = (unsigned char) (page<<1);
  if (addr>0xff) {
    page_temp |=0x01;
  }
  send_spi_byte (page_temp);
  send_spi_byte ((unsigned char) addr);

  // additional bytes
  send_spi_byte(0x00);
  send_spi_byte(0x00);
  send_spi_byte(0x00);
  send_spi_byte(0x00);
}
 
void end_main_array_read(void) {
  nCS_HIGH;
}

void page_to_buffer1_compare(unsigned short page) {
  unsigned char page_temp;
  nCS_LOW;
  send_spi_byte(0x60);           // command for byte write to buff 1
  page_temp = (unsigned char)(page>>7);
  send_spi_byte (page_temp);
  page_temp = (unsigned char) (page<<1);
  send_spi_byte (page_temp);
  send_spi_byte (0x00);
  nCS_HIGH;
}

void buffer1_to_page_e(unsigned short page) {
  unsigned char page_temp;
  nCS_LOW;
  send_spi_byte(0x83);       // 0x83 command for byte write to buff 1
  page_temp = (unsigned char)(page>>7);
  send_spi_byte (page_temp);
  page_temp = (unsigned char) (page<<1);
  send_spi_byte (page_temp);
  send_spi_byte (0x00);
  nCS_HIGH;                 // starts the page erase and program 
}

void buffer2_to_page_e(unsigned short page) {
  unsigned char page_temp;
  nCS_LOW;
  send_spi_byte(0x86);       // 0x86 command for byte write to buff 2
  page_temp = (unsigned char)(page>>7);
  send_spi_byte (page_temp);
  page_temp = (unsigned char) (page<<1);
  send_spi_byte (page_temp);
  send_spi_byte (0x00);
  nCS_HIGH;                 // starts the page erase and program 
}

unsigned char poll_status_register_blocking(void) {  
  unsigned char status;
  status = 0;
  nCS_LOW;  
  //  send_spi_byte(0x57);
  send_spi_byte(0xD7);
  while (!(status&0x80)) { 
    status = send_spi_byte(0x00); // send data to get the spi to clock
  }
  nCS_HIGH;
  return (status); 
}

void buffer1_write(unsigned short addr, unsigned char data_byte) {
  nCS_LOW;
  send_spi_byte(0x84);       // command for byte write to buff 1
  send_spi_byte(0x00);       // dummy byte
  if (addr>0xFF) {
    send_spi_byte(0x01);     // dummy byte except 
  }
  else {
    send_spi_byte(0x00);     // dummy byte except 
  }  
  send_spi_byte((unsigned char) addr); // send the balance of the address
  send_spi_byte(data_byte);
  nCS_HIGH; 
}

void buffer2_write(unsigned short addr, unsigned char data_byte) {
  nCS_LOW;
  send_spi_byte(0x87);       // command for byte write to buff 2
  send_spi_byte(0x00);       // dummy byte
  if (addr>0xFF) {
    send_spi_byte(0x01);     // dummy byte except 
  }
  else {
    send_spi_byte(0x00);     // dummy byte except 
  }  
  send_spi_byte((unsigned char) addr); // send the balance of the address
  send_spi_byte(data_byte);
  nCS_HIGH; 
}

unsigned char buffer1_read(unsigned short addr) {
  unsigned char temp;
  nCS_LOW;
  send_spi_byte(0xD4);       // command for byte write to buff 1
  send_spi_byte(0x00);       // dummy byte
  if (addr>0xFF) {
    send_spi_byte(0x01);     // MSB of buffer address
  }
  else {
    send_spi_byte(0x00);     // MSB of buffer address
  }  
  send_spi_byte((unsigned char) addr); // send the balance of the address
  send_spi_byte(0x00);       // extra clocks required by the flash
  temp=send_spi_byte(0x00);  // this is the data 
  nCS_HIGH; 
  return(temp);
}

unsigned char buffer2_read(unsigned short addr) {
  unsigned char temp;
  nCS_LOW;
  send_spi_byte(0xD6);       // command for byte write to buff 1
  send_spi_byte(0x00);       // dummy byte
  if (addr>0xFF) {
    send_spi_byte(0x01);     // MSB of buffer address
  }
  else {
    send_spi_byte(0x00);     // MSB of buffer address
  }  
  send_spi_byte((unsigned char) addr); // send the balance of the address
  send_spi_byte(0x00);       // extra clocks required by the flash
  temp=send_spi_byte(0x00);  // this is the data 
  nCS_HIGH; 
  return(temp);
}

/* external_flash.c ends here */
