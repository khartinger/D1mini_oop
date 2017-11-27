# D1 mini: Set date and time of RTC DS1307 by Serial
Sketch: D1_oop23_DS1307_setSerial, Version 2017-11-27   
[Deutsche Version](./LIESMICH.md "Deutsche Version")

Set date and time of a DS1307 RTC (Real Time Clock) using the class **`DS1307`**.
* Sending a String `T_yyyymmdd HHMMSS` (followed by new line \n) via Serial sets date and time to the given values yyyy (year) mm (month) dd (day) HH (hour) MM (minute) and SS (seconds).   
Example: T_20171127 185040 sets DS1307 to November 27, 2017 18:50 and 40 secs.   
* Date and time are shown every 2 seconds.

When using Arduino-IDE: Don't forget to change drop down menu to "new line" on the lower right of the serial monitor!   

Default i2c 7bit-address is 0x68.    
All results are printed to Serial (115200 Baud).

## Hardware
* WeMos D1 mini
* data logger shield (= microSD + DS1307RTC Shield)   
  DS1307  uses I2C (D1-SCL, D2-SDA)   
  microSD uses SPI (D5-CLK, D6-MISO, D7-MOSI, D8-CS)   

![Image: D1mini microSD + DS1307 shield](./images/D1_microSD_DS1307_shield.png "D1mini microSD + DS1307 shield")

**Example for Serial output:**
```
RTS DS1307: OK
Arduino-IDE: Don't forget to set drop down 
menu on lower right to "New line" :-)
SO 26.11.2017 03:33:50  | Set time by sending T_yyyymmdd HHMMSS
SO 26.11.2017 03:33:52  | Set time by sending T_yyyymmdd HHMMSS
Set date: OK
Set time: OK
MO 27.11.2017 18:50:40  | Set time by sending T_yyyymmdd HHMMSS
MO 27.11.2017 18:50:42  | Set time by sending T_yyyymmdd HHMMSS
```
