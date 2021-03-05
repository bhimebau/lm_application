# IU Light Sensor Command Language

## Command Overview

The light sensor uses the STM32L4 LPUART for communication. This UART
is special in that it can run when the sensor is in STOP2 low power
mode. The tradeoff for this low power operation is that the UART runs
at 9600 baud. The communication parameters should be set to 8-N-1 (8
bits, no parity, 1 stop bit). 


It is assumed that you are using a USB-UART converter such as many
that are made using the FTDI series of chips. On Linux and OSX, the
screen utility can be used to get access to the serial port. Here is
an example invocation from linux.

```bash
bhimebau@orion:/dev$ screen /dev/ttyUSB0 9600
```

On Windows, consider using [Putty](https://www.putty.org/) as a serial terminal. 

## Commands

A command is a group of ASCII characters that are terminated with a
newline (\n) or a newline/cr (\n\r). Arguments to a command are
appended to the command in a **comma delimited list**. White space is
ignored. 

* **Help:** This command lists all of the command that are available. 
  * Format: help
  * Example Usage: 

``` bash
03/05/2021 07:49:28 IULS> help
Available Commands:
@
ds
ts
tr
dr
data
log
ef
help
ver
tsl237
led
sample
debug
flash
uid
cal
sky
temp
OK
03/05/2021 07:49:37 IULS>
```

* **tsl237** This commands reads from the light sensor without writing
     the result to the sensors flash memory.
    * Format: tsl237,<mode>
      *mode: 
      * raw: reports the period of the TSL237 signal in microseconds (uS)
    * Example Usage: 
        
``` bash
03/05/2021 08:06:33 IULS> tsl237,raw
146
OK
```





* **Attention:** This is a command is used to wake up the system
  through the LPUART. The symbol is sent with the expectation of
  receiving an "OK" when the system has recovered from STOP2.
  * Format: @
  * Successful Example: Wakes the system up from STOP2. Returns OK
  ``` bash
    @
    OK
    IULS>
  ```
  * Failed Example: Wrong character sent.  Will still wake up the
    system but the system will immediately go back to sleep. Returns
    NOK but no prompt - system goes back to sleep.
  ``` bash
    %
    NOK
  ```
  
* **Set date:** Commands to set the real-time clock date
  * Format: ds,*month*,*day*,*year*
  * Successful Example: Sets date to 5/31/2019. Returns OK
  ``` bash
    IULS> ds,5,31,2019
    OK
    IULS>
  ```
  * Failed Example: Missing month field. Returns NOK. 
  ``` bash
    IULS> ds,5,2019
    NOK
    IULS>
  ```
* **Set time:** 
  * Format: ts,*hour*,*min*,*second*
  * Successful Example: Sets time to 13:45:10 (24 hour time) - 10 seconds passed 1:45pm. Returns OK
  ``` bash
    IULS> ts,13,45,10
    OK
    IULS>
  ```
  * Failed Example: Out of range hour field. Returns NOK. 
  ``` bash
    IULS> ts,35,45,10
    NOK
    IULS>
  ```
* **Report time:** Report the current time from the sensor RTC
  * Format: tr 
  * Successful Example: Reads the current time from the board. 
  ``` bash
    IULS> tr
    tr,13,45,10
    OK
    IULS>
  ```
  * Failed Example: Argument passed to the report time command which does not take a command. 
  ``` bash
    IULS> tr,23
    NOK
    IULS>
  ```

* **Report date:** dr
* **Report temperature:** temp
* **Report battery voltage:** batt
* **Report data:** data
* **Erase flash:** ef,all
