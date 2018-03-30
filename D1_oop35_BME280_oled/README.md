# D1 mini with BME280: Show temperature, humidity, pressure and altitude on RS-232
Sketch: D1_oop34_BME280.ino, Version 2018-03-26   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

This simple example measures temperature, humidity, pressure and altitude using a BME280 sensor and shows values on Serial (RS-232, 115200 Baud).   
   
| ___BME280___  | Range                 | Tolerance     |
|:------------- |:---------------------:|:------------- |
| temperature   | -40&deg;C...+85&deg;C | &#x00B1;1&deg;C (0,01&deg;C resolution) |
| humidity      | 0%...100%             | &#x00B1;3%RH   |
| pressure      | 300...1100hPa         | &#x00B1;1,0hPa |   
   
## Hardware
* WeMos D1 Mini
* BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3 (I2C-address 0x76)

**Example for Serial output:**
```
BME280 found!
{"T":21.9,"H":0.0,"p":657.9,"a":3497.0}
21.89 | 0.01 | 658 | 3497.0
T=21.89*C, H=0.01%, p=657.95hPa, a=3496.99m
--------------------------------------------
{"T":27.7,"H":23.2,"p":972.3,"a":347.0}
27.73 | 23.17 | 972 | 347.0
T=27.73*C, H=23.17%, p=972.27hPa, a=346.96m
--------------------------------------------
```
