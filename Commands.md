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
  * Format: **help**
  * Example: 
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

* **tsl237:** This commands reads from the light sensor without writing
     the result to the sensor's flash memory.
   * Format: **tsl237,\<mode\>**
   * mode: determines whether the raw value or a converted magnitude is displayed. 
       * raw: reports the period of the TSL237 signal in microseconds (uS)
       * mag: reports value in mag/arcsec^2 after running through a conversion table. 
   * Example: 
   ``` bash
   03/05/2021 08:06:33 IULS> tsl237,raw
   146
   OK
   ```
   *The sensor is reporting that the period the light sensor square wave is 146uS.* 

* **data:** Read all of the data records stored in the device. This
  includes data that is automatically sampled per the sampling
  schedule (hourly from 5pm to 8am) and also snapshot data that is
  taken using the sample command.
   * Format: **data**
   * Example: 
   ``` bash
   03/05/2021 08:06:33 IULS> data

    ... many records ommitted for brevity ...

   D,1609,03/04/2021,22:00:01,3.454,17,3582,-1
   D,1610,03/04/2021,23:00:01,3.453,17,3577,-1
   D,1611,03/05/2021,00:00:01,3.455,17,3575,-1
   D,1612,03/05/2021,01:00:01,3.454,17,3579,-1
   D,1613,03/05/2021,02:00:01,3.452,16,3578,-1
   D,1614,03/05/2021,03:00:01,3.452,16,3581,-1
   D,1615,03/05/2021,04:00:01,3.451,17,3575,-1
   D,1616,03/05/2021,05:00:01,3.451,16,3581,-1
   D,1617,03/05/2021,06:00:01,3.450,16,3586,-1
   D,1618,03/05/2021,07:00:01,3.451,17,85,-1
   D,1619,03/05/2021,08:00:01,3.469,21,148,-1
   D,1620,03/05/2021,08:08:10,3.485,22,107,-1
   OK
   03/05/2021 09:32:48 IULS> 
   ```

   A single record format can be interpreted as follows: 
   * Record Format
     * **Record Type:** D = Data Record, L = Log Record. Always D from the data command. 
     * **Record Number:** Specific record number in the flash. These
       will alway be increasing. However, they may go up by more than
       1 for each record. This could be caused by a log record being
       written between data records.
     * **Date:** Date that the sample was completed. If the sample was
       started near the end of a day and the date changed before the
       sample completed, this date reflects when it finished.
     * **Time:** 24 Hour time that represents when the sample
       completed. In the case of a scheduled sample, it is possible to
       determine how long the sample started based in the time stamp.
     * **Battery Voltage:** Loaded battery terminal voltage. The
       battery voltage can only be measured when the tsl237 sensor is
       powered. Because of the relatively high ESR of the battery,
       this additional load causes the terminal voltage to read about
       100 mV lower than it is when the sensor is sleeping.
     * **Temperature:** Temperature when the sample was taken. This
       measurement is taken from the STM32L432's internal temperature
       sensor. The measurement is +/- 1 Deg C.
     * **Period:** Period of the tsl237 signal in microseconds. 
     * **Magnitude:** Period converted to mag/arcsec^2. A -1 indicates
       that the sample was brighter than the lowest value in the table
       (15.3). A 1 indicates that the sample was darker than the
       highest value in the table (24.0).

* **sample:** Force the sensor to take a sample now and store it to
  flash. To read the data, use the data command. In darker
  environments, this command can take minutes to return.
  * Format: sample
  * Example: 

   ``` bash
   03/05/2021 08:08:10 IULS> sample
   OK
   03/05/2021 08:32:46 IULS> 
   ```

* **Attention:** This is a command is used to confirm that the system
  is connected. Just returns OK if successful. Presumably, nothing
  would be received in response if the system was not connected.
  * Format: @
  * Example: 

   ``` bash
   03/05/2021 08:08:10 IULS> @
   OK
   03/05/2021 08:32:46 IULS> 
   ```
  
* **Set date:** Set the real-time clock date
  * Format: ds,\<month\>,\<day\>,\<year\>
  * Example: Sets date to 5/31/2019. Returns OK
  ``` bash
    03/05/2021 08:08:10 IULS> ds,5,31,2019
    OK
    05/31/2019 08:32:46 IULS>
  ```

* **Set time:** Set the real-time clock time
  * Format: ts,\<hour\>,\<min\>,\<second>\
  * Example: Sets time to 13:45:10 (24 hour time) - 10 seconds passed 1:45pm. Returns OK
  ``` bash
  03/05/2021 08:08:10 IULS> ts,13,45,10
  OK
  03/05/2021 13:45:10 IULS> 
  ```

* **Report time:** Report the current time from the sensor RTC
  * Format: tr 
  * Example: 
  ``` bash
  03/05/2021 09:24:08 IULS> tr
  tr,09,27,18
  OK
  03/05/2021 09:27:18 IULS> 
  ```

* **Report Date:** Report the current date from the sensor RTC
  * Format: td
  * Example: 
  ``` bash
  03/05/2021 09:28:48 IULS> dr
  dr,03,05,2021
  OK
  03/05/2021 09:28:54 IULS> 
  ```

* **Report date:** dr
* **Report temperature:** temp
* **Report battery voltage:** batt
* **Report data:** data
* **Erase flash:** ef,all
