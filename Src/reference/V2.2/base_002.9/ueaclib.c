/* ueaclib.c --- 
 * 
 * Filename: ueaclib.c
 * Description: library of utilities needed to interact with the uEAC R002
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Wed Nov  2 11:24:10 2005
 * Version: 
 * Last-Updated: Tue Nov  8 11:46:20 2005
 *           By: Bryce Himebaugh
 *     Update #: 105
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
// Header file for the IUCS uEAC R002 hardware
// 6/1/05 initial version BH

#include <msp430x16x.h>
#include <signal.h>
#include <stdio.h>
#include "ueaclib.h"
#include "ueac.h"
#include "external_flash.h"
#include "interpreter.h"
#include "filter.h"
#include "conversion.h"
#include "global.h"
#include "timer.h"
#include "calibrate.h"

extern short dac_translation[];

#define DELTA 900                   // target DCO = DELTA*(4096) = 3686400

// Function Prototypes
void init_pins(void);
void init_spi_0 (void);
void init_serial_1 (void);
void init_timer_a(void);
void init_dac(void);
void init_a2d(void);
void start_a2d_converter (void);
void Set_DCO (void);
void clear_latches(void);
void system_reset(void); 


void init_sys(void) {
  unsigned int i;
  WDTCTL = WDTPW + WDTHOLD;    // Stop watchdog
  init_pins();                 // Setup the discrete I/O - important to enable 8Mhz crystal 
  for (i = 0xFFFF;i>0;i--);    // Delay for XTAL and oscillator to fire up and settle
  Set_DCO();                   // calibrate DCO using the 32.768Khz crystal to 3.686400 Mhz  
  init_spi_0();
  init_serial_1();             // initialize USB virtual serial port
  init_timer_a();              
  init_dac();
  init_a2d();
  clear_latches();
  _EINT();                     // Global interrupt enable
}

void system_reset(void) {
  _DINT();
  WDTCTL = WDTPW|WDTCNTCL|WDTIS_3;  // start the watchdog, SMCLK ticks until reset 
  P4OUT=0x8F;
  while (1);                        // wedge here until the dog resets the core
}

void clear_latches(void) {
  P1OUT=0x00;
  P5OUT=0x3F;
  P5OUT=0x00;
}

void Set_DCO (void) { 
  unsigned int Compare, Oldcapture = 0;
  CCTL0 = 0x0000;                       // this too - see next comment
  CCTL1 = 0x0000;                       // see if this fixes the clock cal issue
  CCTL2 = 0x0000;
  CCTL2 = CCIS0 + CM0 + CAP;            // Define CCR2, CAP, ACLK
  TACTL = TASSEL1 + TACLR + MC1;        // SMCLK, continous mode
  while (1) {
    while ((CCTL2 & CCIFG) != CCIFG);   // Wait until capture occured!
    CCTL2 &= ~CCIFG;                    // Capture occured, clear flag
    Compare = CCR2;                     // Get current captured SMCLK
    Compare = Compare - Oldcapture;     // SMCLK difference
    Oldcapture = CCR2;                  // Save current captured SMCLK
    if (DELTA == Compare) {
      break;                            // if equal, leave "while(1)"
    }
    else if (DELTA < Compare) {         // DCO is too fast, slow it down
      DCOCTL--;
      if (DCOCTL == 0xFF) {             // Did DCO roll under?
	BCSCTL1--;                      // Select next lower RSEL
      }
    }
    else {                      
      DCOCTL++;
      if (DCOCTL == 0x00) {             // Did DCO roll over?
        BCSCTL1++;                      // Select next higher RSEL
      }
    }
  }  
  CCTL2 = 0;                              // Stop CCR2 function
  TACTL = 0;                              // Stop Timer_A
}

void init_spi_0 (void) {
  UCTL0 |= SWRST;                // Place USART into RESET 
  UCTL0  = CHAR|SYNC|MM|SWRST;   // 8-bit,SPI,Master,Hold Module in RESET
  UTCTL0 = CKPH|SSEL1|SSEL0|STC; // falling edge out,SMCLK,3-pin SPI (Works for LTC Parts)
  // UTCTL0 = CKPL|SSEL1|SSEL0|STC; // falling edge out,SMCLK,3-pin SPI (Works for AT parts)
  // LTC1660 DAC can handle a 5Mhz SCLK
  // AT45DB041 Flash can handle 20Mhz SCLK 
  UBR00  = 0x02;                 // Run at SMCLK/2 - 8Mhz/2=4Mhz for normal operation
  UBR10  = 0x00;                 // Upper half of SCLK control 
  ME1    = USPIE0;               // Enable the SPI module for UART0
  UMCTL0 = 0x00;                 // Modulation control not used by SPI set to 0 according to User's Guide
  UCTL0 &= ~SWRST;               // release USART from RESET 
}

unsigned char send_spi_byte(unsigned char data_byte) {
  TXBUF0 = data_byte;        // buffer 1 write  
  while(!(UTCTL0&0x01));     // wait until transmitter empty
  return(RXBUF0);            // return any received data
                             // No data returned from DAC
                             // SPI flash returns read data
}

void init_serial_1 (void) {
  char temp;
  // Data Comm Port - Connected to FT2232 Port A
  UCTL1 = CHAR + SWRST;
  UTCTL1 = SSEL1 + SSEL0;

  // 19.2k init (3.686400 Mhz Clock)
  UBR01=0xC0; 
  UBR11=0x00;
  UMCTL1=0x00; 
  ME2 = 0x30;
  UCTL1 = CHAR;
  temp=RXBUF1;  // Flush the RX buffer 
  temp=RXBUF1;  
}
    
int putchar(int in_char){
  while (!(IFG2&UTXIFG1));
  TXBUF1=in_char;
  return(0);
}

char getchar(void) {
  char rx_data;
  while (!(IFG2&URXIFG1));
  rx_data= RXBUF1;
  return (rx_data);
}

void init_pins(void) {
 /*!
  * <i><b>P1 Usage</b></i> 
  * <table>
  * <tr><th>Pin<th>Function<th>Direction<th>Select
  * <tr><td>P1.0<td>Latch_Data(0)<td>Output<td>Dio  
  * <tr><td>P1.1<td>Latch_Data(1)<td>Output<td>Dio    
  * <tr><td>P1.2<td>Latch_Data(2)<td>Output<td>Dio    
  * <tr><td>P1.3<td>Latch_Data(3)<td>Output<td>Dio    
  * <tr><td>P1.4<td>Latch_Data(4)<td>Output<td>Dio    
  * <tr><td>P1.5<td>Latch_Data(5)<td>Output<td>Dio    
  * <tr><td>P1.6<td>Latch_Data(6)<td>Output<td>Dio    
  * <tr><td>P1.7<td>Latch_Data(7)<td>Output<td>Dio    
  * </table>
  */
  P1SEL = 0x00;
  P1OUT = 0x00;
  P1DIR = 0xFF;

 /*!
  * <i><b>P2 Usage</b></i> 
  * <table>
  * <tr><th>Pin<th>Function<th>Direction<th>Select
  * <tr><td>P2.0<td>LED_OUT(24)<td>Output<td>Dio    
  * <tr><td>P2.1<td>LLA_ENABLE(24)<td>Output<td>Dio    
  * <tr><td>P2.2<td>BSL(0) FTDI CHB TXD<td>Input<td>Dio  
  * <tr><td>P2.3<td>No Connect<td>Output<td>Dio    
  * <tr><td>P2.4<td>No Connect<td>Output<td>Dio    
  * <tr><td>P2.5<td>100k Pull-up Rosc<td>Input<td>Dio    
  * <tr><td>P2.6<td>No Connect<td>Output<td>Dio    
  * <tr><td>P2.7<td>FTDI nRTS<td>Input<td>Dio    
  * </table>
  */
  P2SEL = 0x00;
  P2OUT = 0x00;
  P2DIR = 0x5B;

 /*!
  * <i><b>P3 Usage</b></i> 
  * <table>
  * <tr><th>Pin<th>Function<th>Direction<th>Select
  * <tr><td>P3.0<td>FTDI nCTS<td>Output<td>Dio
  * <tr><td>P3.1<td>SIMO-0<td>Output<td><b>Sel</b>
  * <tr><td>P3.2<td>SOMI-0<td>Input<td><b>Sel</b>
  * <tr><td>P3.3<td>SCK-0<td>Output<td><b>Sel</b>
  * <tr><td>P3.4<td>UART0 TX<td>Output<td><b>Sel</b>
  * <tr><td>P3.5<td>UART0 RX<td>Input<td><b>Sel</b>
  * <tr><td>P3.6<td>UART1 TX<td>Output<td><b>Sel</b>
  * <tr><td>P3.7<td>UART1 RX<td>Input<td><b>Sel</b>
  * </table>
  */
  P3SEL = 0xFE;
  P3OUT = 0x00;
  P3DIR = 0x5B;

 /*!
  * <i><b>P4 Usage</b></i> 
  * <table>
  * <tr><th>Pin<th>Function<th>Direction<th>Select
  * <tr><td>P4.0<td>SPI nCS DAC CH 0-7<td>Output<td>Dio
  * <tr><td>P4.1<td>SPI nCS DAC CH 8-15<td>Output<td>Dio
  * <tr><td>P4.2<td>SPI nCS DAC CH 16-23<td>Output<td>Dio
  * <tr><td>P4.3<td>SPI nCS Serial Flash<td>Output<td>Dio
  * <tr><td>P4.4<td>Analog Mux Select (0)<td>Output<td>Dio
  * <tr><td>P4.5<td>Analog Mux Select (1)<td>Output<td>Dio
  * <tr><td>P4.6<td>Analog Mux Select (2)<td>Output<td>Dio
  * <tr><td>P4.7<td>8Mhz Osc Enable<td>Output<td>Dio
  * </table>
  */
  P4SEL = 0x00;
  P4OUT = 0x8F;   
  P4DIR = 0xFF; 
 /*!
  * <i><b>P5 Usage</b></i> 
  * <table>
  * <tr><th>Pin<th>Function<th>Direction<th>Select
  * <tr><td>P5.0<td>LED Latch Clk CH 0-7<td>Output<td>Dio
  * <tr><td>P5.1<td>LED Latch Clk CH 8-15<td>Output<td>Dio
  * <tr><td>P5.2<td>LED Latch Clk CH 16-23<td>Output<td>Dio
  * <tr><td>P5.3<td>LLA Latch Clk CH 0-7<td>Output<td>Dio
  * <tr><td>P5.4<td>LLA Latch Clk CH 8-15<td>Output<td>Dio
  * <tr><td>P5.5<td>LLA Latch Clk CH 16-23<td>Output<td>Dio
  * <tr><td>P5.6<td>No Connect<td>Output<td>Dio
  * <tr><td>P5.7<td>No Connect<td>Output<td>Dio
  * </table>
  */
  P5SEL = 0x00;
  P5OUT = 0x00;
  P5DIR = 0xFF; 

 /*!
  * <i><b>P6 Usage</b></i> 
  * <table>
  * <tr><th>Pin<th>Function<th>Direction<th>Select
  * <tr><td>P6.0<td>VMUX_OUT(0)<td>Input<td><b>Sel</b>
  * <tr><td>P6.1<td>VMUX_OUT(1)<td>Input<td><b>Sel</b>
  * <tr><td>P6.2<td>VMUX_OUT(2)<td>Input<td><b>Sel</b>
  * <tr><td>P6.3<td>VMUX_OUT(3)<td>Input<td><b>Sel</b>
  * <tr><td>P6.4<td>VMUX_OUT(4)<td>Input<td><b>Sel</b>
  * <tr><td>P6.5<td>No Connect<td>Input<td>Dio
  * <tr><td>P6.6<td>DAC Control CH24<td>Input (analog output)<td><b>Sel</b>
  * <tr><td>P6.7<td>No Connect<td>Input<td>Dio
  * </table>
  */
  P6SEL = 0x5F;
  P6OUT = 0x00;
  P6DIR = 0xA0; 
}

void init_timer_a(void) {
  // SMCLK SOURCE (3.686400Mhz)
  // Timer in Continuous Mode 
  // Clear the timer register (TAR)
  // Compare 1 used as main time base at 1.25mS. 
  TACTL=TASSEL1|MC1|TACLR;  // Timer a sourced from 3.686400 Mhz SMCLK, continuous mode
  TACCR0=0x900;             // 100uS interrupt rate
  TACCTL0=CCIE;             // Enable the timer interrupt
}

void init_dac(void) {
  // Dac 0 Controls Sources 
  // Dac 1 Controls Sinks
  // Parameters
  // [14-13] DAC12SREFx (11) Use VeREF+->3.3V 
  // [12] DAC12RES (0) 12-Bit Resolution  
  // [11-10] DAC12LSELx (00) Load DAC on write to the DAC12_0DAT register
  // [9] DAC12CALON (1) Calibration active, poll until this bit is clear 
  // [8] DAC12IR (1) Input range = 1x
  // [7-5] DAC12AMPx (111) High Speed, High Current 
  // [4] DAC12DF (0) Straight Binary 
  // [3] DAC12IE (0) Interrupt Disabled
  // [2] DAC12IFG (x) Interrupt flag
  // [1] DAC12ENC (1) Enable DAC conversion 
  // [0] DAC12GRP (0) Channel grouping disabled 
  DAC12_0CTL = DAC12SREF1|DAC12SREF0|DAC12CALON|DAC12IR|DAC12AMP2|DAC12AMP1|DAC12AMP0|DAC12ENC;
  while (DAC12_0CTL&DAC12CALON);  // spin here until cal complete
}

/*!  ADC12 Initialization Function
 * \section AA Overview
 * This project utilizes the first 5 channels of the A/D. Each of the channels is setup to use the 3.3V supply as the 
 * Reference. <br>
 *
 * \section BB Converter Usage
 * <table> 
 * <tr><th>Pin<th>Function<th>Description
 * <tr><td>P6.0<td>A/D 0<td>Reads External Mux for CH 0-7
 * <tr><td>P6.1<td>A/D 1<td>Reads External Mux for CH 8-15
 * <tr><td>P6.2<td>A/D 2<td>Reads External Mux for CH 16-23
 * <tr><td>P6.3<td>A/D 3<td>Reads CH24 
 * <tr><td>P6.3<td>A/D 4<td>Reads VOP Voltage 10k//2.5k divider 
 * </table>
 * \section CC VOP Calculation
 * VOP_Voltage = AD_Counts_VOP/248  
 * \section D Converter Sampling Time Usage
 * t_sample>(Rs+2k)x9.011x40pf+800nS; Rs is the channel external input resistance - 200 ohms for uEAC<br>
 * t_sample>2200x9.011x40pf+800nS=1.6uS 
 * 1.6uS = 12.8 clocks of the 8Mhz oscillator. Choose 16 for the sample clock divisor. 
 */ 
void init_a2d (void) {
  // ADC12 Parameters
  // Sample Hold Time - 16 clocks 
  /*! \section D ADC12CTRL0 Config 
  */
  ADC12CTL0 = SHT1_2|SHT0_2|MSC|REF2_5V|REFON|ADC12ON; // sampling time set to 3.2uS 
  ADC12CTL1 = SHP|CONSEQ0;                             // sample sequence of channels and then stop 
  ADC12MCTL0 =  SREF_2|INCH_0;                         // Use Avcc (3.3V) as the reference, channels (0-7)   
  ADC12MCTL1 =  SREF_2|INCH_1;                         // Use Avcc (3.3V) as the reference, channels (8-15)   
  ADC12MCTL2 =  SREF_2|INCH_2;                         // Use Avcc (3.3V) as the reference, channels (16-23)   
  ADC12MCTL3 =  EOS|SREF_2|INCH_3;                     // Use Avcc (3.3V) as the reference, channels (24)    
}

void start_a2d_converter(void) {
  ADC12CTL0|=ENC|ADC12SC;                    // start the converter 
}  

void stop_a2d_converter(void) {
  ADC12CTL0&=~ENC;                           // stop the converter 
}  

int wait_a2d_busy(void) {
  int count=0;
  while (ADC12CTL1&ADC12BUSY) count++;
  return count;
}

void write_analog_mux(unsigned char select) {
  select&=0x07;   // clamp the input to 7 
  select<<=4;     // shift to align with bits 6-4 (mux select bits)
  P4OUT&=0x8F;    // clear the mux select bits
  P4OUT|=select;  // "or" in the new select value
}

void write_current(int channel,int value_uA) {
  if (value_uA>200) value_uA=200;
  if (value_uA<-200) value_uA=-200;
  if (value_uA==0) {
    write_dac(channel,(dac_translation[200]+ueac_state.pin_cal[channel].i_zero_offset));
  }
  else {
    write_dac(channel,dac_translation[200-value_uA]+ueac_state.pin_cal[channel].i_200uA_offset);
  }
}

void write_dac(int channel,int value) {
  // write_dac(channel,value)
  // channel = 0-24 where the channels are the control voltages for the current sources. The current 
  // sources are labeled starting at the top left corner as follows.
  //  0  1  2  3  4
  //  5  6  7  8  9
  // 10 11 12 13 14
  // 15 16 17 18 19
  // 20 21 22 23 24
  //
  // Value = 0-1023 (10-bit) where the number represents a control voltage that is 0-3.3v. Each bit represents 
  // a voltage of 3.3v/1023 or 3.22mV. 
  //
  // Hardware Note: Channels 0-23 are implemented by external SPI octal dacs (Linear LTC1660 components). Channel 
  // 24 is implemented using DAC0 on the MSP430 
  // 
  // Initial Version BH 11/1/05

  if (channel < 8) {
    P4OUT&=~0x01;                                   // assert the proper chip select
    channel++;                                      // increment the channel number LTC1660 channels run from 1-8   
  }           
  else if (channel < 16) {
    P4OUT&=~0x02;                                   // assert the proper chip select
    channel-=7;                                     // bring channel number into range 1-8
  } 
  else if (channel < 24) {
    P4OUT&=~0x04;                                   // assert the proper chip select
    channel-=15;                                    // bring channel number into range 1-8
  }
  else if (channel == 24) {
    DAC12_0DAT=value<<2;                            // Shift up to a 12-bit number and write to the MSP430 DAC0 
    return;                                         // This is all to do for the MSP430 DAC case, exit...
  }
  else {                                            // channel number provided is too large, exit... 
    return;
  }
  value=(value&0x03FF)<<2;                          // mask and shift the value to align properly in the dac sentence
  channel<<=4;
  *(((unsigned char *) &value)+1)|=((unsigned char) channel); // "or" in the channel number to the value data
  send_spi_byte(*(((unsigned char *) &value)+1));             // send the high byte 
  send_spi_byte(*((unsigned char *) &value));                 // send the high byte 
  P4OUT|=0x0F;                                                // deassert all of the spi dac chip selects   
}

#define PWM_COUNT_MASK 0x1F
void led_pwm (int enable) {
  static unsigned char counter=0;
  if (enable) {
    counter++;
    counter&=PWM_COUNT_MASK; 
    if (!counter) {
      P1OUT=0xFF;
      P5OUT=0x07;
      P5OUT=0;
      P2OUT|=0x01;
    }
    P1OUT=0xFF;
    if (counter>=high_time_limit[0]) P1OUT&=~0x01;
    if (counter>=high_time_limit[1]) P1OUT&=~0x02;
    if (counter>=high_time_limit[2]) P1OUT&=~0x04;
    if (counter>=high_time_limit[3]) P1OUT&=~0x08;
    if (counter>=high_time_limit[4]) P1OUT&=~0x10;
    if (counter>=high_time_limit[5]) P1OUT&=~0x20;
    if (counter>=high_time_limit[6]) P1OUT&=~0x40;
    if (counter>=high_time_limit[7]) P1OUT&=~0x80;
    P5OUT=0x01;
    P5OUT=0x00;
    P1OUT=0xFF;
    if (counter>=high_time_limit[8]) P1OUT&=~0x01;
    if (counter>=high_time_limit[9]) P1OUT&=~0x02;
    if (counter>=high_time_limit[10]) P1OUT&=~0x04;
    if (counter>=high_time_limit[11]) P1OUT&=~0x08;
    if (counter>=high_time_limit[12]) P1OUT&=~0x10;
    if (counter>=high_time_limit[13]) P1OUT&=~0x20;
    if (counter>=high_time_limit[14]) P1OUT&=~0x40;
    if (counter>=high_time_limit[15]) P1OUT&=~0x80;
    P5OUT=0x02;
    P5OUT=0x00;
    P1OUT=0xFF;
    if (counter>=high_time_limit[16]) P1OUT&=~0x01;
    if (counter>=high_time_limit[17]) P1OUT&=~0x02;
    if (counter>=high_time_limit[18]) P1OUT&=~0x04;
    if (counter>=high_time_limit[19]) P1OUT&=~0x08;
    if (counter>=high_time_limit[20]) P1OUT&=~0x10;
    if (counter>=high_time_limit[21]) P1OUT&=~0x20;
    if (counter>=high_time_limit[22]) P1OUT&=~0x40;
    if (counter>=high_time_limit[23]) P1OUT&=~0x80;
    P5OUT=0x04;
    P5OUT=0x00;
    if (counter>=high_time_limit[24]) P2OUT&=~0x01;
  }
}

void clear_led_screen (void) {
  P1OUT=0;
  P5OUT=0x07;
  P5OUT=0;
  P2OUT&=~0x01;
}


void write_led (int channel, int enable) {
  static unsigned long led_state = 0x00000000;
  if (enable) {
    if (channel<8) {
      *((unsigned char *) &led_state) |= 0x01<<channel;       // Save the new state of the LED in led_state
      P1OUT=*((unsigned char *) &led_state);                  // Place the relevant byte of led_state on the latch bus 
      P5OUT|=0x01;                                            // strobe the chip select for the target latch
      P5OUT&=~0x01;                                           // clear the chip select bit
    }
    else if (channel<16) {
      channel-=8;
      *(((unsigned char *) &led_state)+1) |= 0x01<<channel;  
      P1OUT=*(((unsigned char *) &led_state)+1);  
      P5OUT=0x02;
      P5OUT&=~0x02;                                           // clear the chip select bit
    }
    else if (channel<24) {
      channel-=16;
      *(((unsigned char *) &led_state)+2) |= 0x01<<channel;  
      P1OUT=*(((unsigned char *) &led_state)+2);  
      P5OUT=0x04;
      P5OUT&=~0x04;                                           // clear the chip select bit
    }
    else if (channel==24) {
      *(((unsigned char *) &led_state)+3) |= 0x01;  
      P2OUT|=0x01;
    }
  }
  else {
    if (channel<8) {
      *((unsigned char *) &led_state) &= ~(0x01<<channel);  
      P1OUT=*((unsigned char *) &led_state);  
      P5OUT=0x01;
      P5OUT&=~0x01;                                           // clear the chip select bit
    }
    else if (channel<16) {
      channel-=8;
      *(((unsigned char *) &led_state)+1) &= ~(0x01<<channel);  
      P1OUT=*(((unsigned char *) &led_state)+1);  
      P5OUT=0x02;
      P5OUT&=~0x02;                                           // clear the chip select bit
    }
    else if (channel<24) {
      channel-=16;
      *(((unsigned char *) &led_state)+2) &= ~(0x01<<channel);  
      P1OUT=*(((unsigned char *) &led_state)+2);  
      P5OUT=0x04;
      P5OUT&=~0x04;                                           // clear the chip select bit
    }
    else if (channel==24) {
      *(((unsigned char *) &led_state)+3) &= 0xFE;  
      P2OUT&=0xFE;
    }
  }
}

void write_lla (int channel, int enable) {
  static unsigned long lla_state = 0x00000000;
  if (enable) {
    if (channel<8) {
      *((unsigned char *) &lla_state) |= 0x01<<channel;       // Save the new state of the LED in lla_state
      P1OUT=*((unsigned char *) &lla_state);                  // Place the relevant byte of lla_state on the latch bus 
      P5OUT|=0x08;                                            // strobe the chip select for the target latch
      P5OUT&=~0x08;                                           // clear the chip select bit
    }
    else if (channel<16) {
      channel-=8;
      *(((unsigned char *) &lla_state)+1) |= 0x01<<channel;  
      P1OUT=*(((unsigned char *) &lla_state)+1);  
      P5OUT=0x10;
      P5OUT&=~0x10;                                           // clear the chip select bit
    }
    else if (channel<24) {
      channel-=16;
      *(((unsigned char *) &lla_state)+2) |= 0x01<<channel;  
      P1OUT=*(((unsigned char *) &lla_state)+2);  
      P5OUT=0x20;
      P5OUT&=~0x20;                                           // clear the chip select bit
    }
    else if (channel==24) {
      *(((unsigned char *) &lla_state)+3) |= 0x01;  
      P2OUT|=0x02;
    }
  }
  else {
    if (channel<8) {
      *((unsigned char *) &lla_state) &= ~(0x01<<channel);  
      P1OUT=*((unsigned char *) &lla_state);  
      P5OUT=0x08;
      P5OUT&=~0x08;                                           // clear the chip select bit
    }
    else if (channel<16) {
      channel-=8;
      *(((unsigned char *) &lla_state)+1) &= ~(0x01<<channel);  
      P1OUT=*(((unsigned char *) &lla_state)+1);  
      P5OUT=0x10;
      P5OUT&=~0x10;                                           // clear the chip select bit
    }
    else if (channel<24) {
      channel-=16;
      *(((unsigned char *) &lla_state)+2) &= ~(0x01<<channel);  
      P1OUT=*(((unsigned char *) &lla_state)+2);  
      P5OUT=0x20;
      P5OUT&=~0x20;                                           // clear the chip select bit
    }
    else if (channel==24) {
      *(((unsigned char *) &lla_state)+3) &= 0xFE;  
      P2OUT&=0xFD;
    }
  }
}

/* ueaclib.c ends here */

