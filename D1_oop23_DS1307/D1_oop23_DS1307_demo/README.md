# D1 mini: Example how to use class DS1307
Sketch: D1_oop23_DS1307_demo, Version 2017-11-27   
[Deutsche Version](./LIESMICH.md "Deutsche Version")

This example shows how to use the class **`DS1307`** to exchange data with a real time clock (RTC) DS1307 via i2c interface. The class is just for communication with the DS1307 clock and does not regard daylight saving time (= summer time), etc.  


**Program details**   
Default i2c 7bit-address is 0x68.    
All results are printed to Serial (115200 Baud).
 * check for i2c answer from device (ask for its address)
 * set date and time values shortly before "midnight"
 * clear RAM (with default value 0) and print the result
 * write a byte and a int value to RAM, read them back and print the result
 * read weekday, date, time and seconds from 1.1.1970 from RTC every 2 seconds for 5 times 
 * get the last values read and print them (without RTC access, just for check)   


If you want to check the function of the battery on data logger shield:
* comment out the lines for setting date and time
* compile program and load it to D1 mini
* remove power supply from D1 mini + data logger shield
* connect to power supply and read values from RTC


## Hardware
* WeMos D1 mini
* data logger shield (= microSD + DS1307RTC Shield)   
  DS1307  uses I2C (D1-SCL, D2-SDA)   
  microSD uses SPI (D5-CLK, D6-MISO, D7-MOSI, D8-CS)   

![Image: D1mini microSD + DS1307 shield](./images/D1_microSD_DS1307_shield.png "D1mini microSD + DS1307 shield")

**Example for Serial output:**
```
class DS1307 demo:
i2c-status for 0x68 is OK
Winterzeit!
RAM clearing ok.
RAM set byte ok.
RAM set int16 ok.
-----Data from DS1307----------------------
SA 25.11.2017 23:59:56 = 20171125 235956 = 1511654396
Last values (should be the same):
SA 25.11.2017 23:59:56 = 20171125 235956 = 1511654396
...
-----Data from DS1307----------------------
SO 26.11.2017 00:00:04 = 20171126 000004 = 1511654404
Last values (should be the same):
SO 26.11.2017 00:00:04 = 20171126 000004 = 1511654404
Finished :)
```
