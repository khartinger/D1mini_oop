# D1 mini: Test of the class `Timeconverter1` 
Sketch: D1_oop83_test_timeconverter1.ino, version 2021-01-29   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   

This programme is used to test methods of the class `Timeconverter1`. The result of the method calls is displayed via the serial interface (115200 baud).    
The class `Timeconverter1` provides methods for converting various date and time formats.   
Examples are   
* English date without hyphens, e.g. 20210129
* English date with hyphens, e.g. 2021-01-29
* German date notation, e.g. 29.01.2021
* Time without colons, e.g. 235959
* Time with colons, e.g. 23:59:59   

Other methods are   
* to calculate the number of days between two dates,
* to add seconds or days to the current date, 
* to determine whether a year is a leap year or
* whether daylight saving time applies to a particular date.

__Output of the programme__
```
setup(): --Start--
-----Date and time should be 20210131 010203---
Seconds since 1970-01-01: 1612054923 (OK)
getYmdHMS():  20210131 010203
getYmdHMS_(): 2021-01-31 01:02:03        
getTmJHMS_(): 31.01.2021 01:02:03
-----show date only-------------------------
getYmd():     20210131
getYmd():     yyyy=2021 mm=1 dd=31
getYmd_():    2021-01-31
getTmJ_():    31.01.2021
-----show time only-------------------------
getHMS():     010203
getHMS():     HH=1 MM=2 SS=3
getHMS_():    01:02:03
getHM_():     01:02
-----working methods------------------------
-----add 29 days => 2021-03-01--------------
getYmd_():    2021-03-01
Difference in Days    : -29
Difference in Days TC2: -29
-----add 82676 seconds => 23:59:59----------
getYmdHMS_(): 2021-03-01 23:59:59
-----add 1 second => 2021-03-02 00:00:00----
getYmdHMS_(): 2021-03-02 00:00:00
getWeekday (DI): DI
-----summertime-----------------------------
1616896799 (OK) = 20210328 015959 (MEZ)  is MEZ  (OK)
1616896800 (OK) = 20210328 020000 (MESZ) is MESZ (OK)
1616896801      = 20210328 020001 (MESZ) is MESZ (OK)
1635649199 (OK) = 20211031 025959 (MESZ) is MESZ (OK)
1635649200 (OK) = 20211031 030000 (MEZ)  is MEZ  (OK)
1635649201      = 20211031 030001 (MEZ)  is MEZ  (OK)
setup(): --Finished--
```

### Hardware 
1. WeMos D1 mini or ESP32 D1 mini   
