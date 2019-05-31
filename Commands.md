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
