* **Set date:** ds,*month*,*day*,*year*
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
* **Report time:** tr 
* **Report date:** dr
* **Report data:** data
* **Erase flash:** ef,all
