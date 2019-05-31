# IU Light Sensor Command Language

## Command Overview

The light sensor will be using the LPUART for communication. The
LPUART is special in that it can accept characters in the STOP modes.

## Commands

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
* **Set time:** ts,*hour*,*min*,*second*
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

* **Report time:** tr 
* **Report date:** dr
* **Report data:** data
* **Erase flash:** ef,all