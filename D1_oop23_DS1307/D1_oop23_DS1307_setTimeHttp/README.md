# D1 mini: Set date and time of RTC DS1307 by Server
Sketch: D1_oop23_DS1307_setTimeHttp, Version 2017-11-27   
[Deutsche Version](./LIESMICH.md "Deutsche Version")

Demo for class **`TimeHttp`**: Set date and time of a DS1307 RTC by sending a HTTP 1.1 request (port 80) to a server with missing "Host: " in header and waiting for an answer.   

__*Don't forget to change WiFi data to your network values*__ in line   
`TimeHttp time1("..ssid..", "..password..","httpservername");`  

**Using this program:**
* Sending character s by Serial sets date+time by HTTP-Request
* Sending character r by Serial resets date+time to a default value   
(sDateTime0="20161231 235954")
* Date and time are printed to Serial every 2 secs.   

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
RTC DS1307: OK
Starting a HTTP-request...
Disconnected. Try to reconnect...

Connecting to Raspi11
............
Connected! IP address is 192.168.1.169
Connected to Wifi!
Connected to server!
Normal time. DS1307 set time: OK
Setup: setHttpTime = 20171127 210208
MO 27.11.2017 21:02:08 | Serial: s sends HTTP-Request, r resets time
MO 27.11.2017 21:02:10 | Serial: s sends HTTP-Request, r resets time
MO 27.11.2017 21:02:12 | Serial: s sends HTTP-Request, r resets time
DS1307 reset time: OK
SA 31.12.2016 23:59:54 | Serial: s sends HTTP-Request, r resets time
SA 31.12.2016 23:59:56 | Serial: s sends HTTP-Request, r resets time
SA 31.12.2016 23:59:58 | Serial: s sends HTTP-Request, r resets time
SO 01.01.2017 00:00:00 | Serial: s sends HTTP-Request, r resets time
SO 01.01.2017 00:00:02 | Serial: s sends HTTP-Request, r resets time
Starting a HTTP-request...
Connected to Wifi!
Connected to server!
Normal time. DS1307 set time: OK
MO 27.11.2017 21:02:24 | Serial: s sends HTTP-Request, r resets time
MO 27.11.2017 21:02:26 | Serial: s sends HTTP-Request, r resets time
MO 27.11.2017 21:02:28 | Serial: s sends HTTP-Request, r resets time
```
