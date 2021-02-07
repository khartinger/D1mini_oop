# D1 mini with BME280: Show temperature, humidity, pressure and altitude on RS-232
Sketch: D1_oop34_BME280.ino, Version 2021-02-07   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   

This sample programme shows how to measure temperature, humidity, air pressure and altitude using the BME280 sensor. The values are measured at intervals of 2 seconds and displayed via the serial interface (RS-232, 115200 baud).   
If you use a BMP280 sensor instead of the BME280, the humidity value is always 0, because this sensor cannot measure humidity ;) 
   
| ___BME280___  | Range                 | Tolerance     |
|:------------- |:---------------------:|:------------- |
| temperature   | -40&deg;C...+85&deg;C | &#x00B1;1&deg;C (0,01&deg;C resolution) |
| humidity      | 0%...100%             | &#x00B1;3%RH   |
| pressure      | 300...1100hPa         | &#x00B1;1,0hPa |   
   
__Note__   
The first value read in is often incorrect and should not be used ;)   
   
## Hardware
* WeMos D1 Mini
* BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3 (I2C-address 0x76)

**Example for Serial output:**
```
setup(): --Start--
BME280 found!        
I2C address: 0x76    
setup(): --Finished--

Measurement started!
New Result: {"T":24.2,"H":32.2,"p":959.7,"a":456.0}
Old Result: 24.23 | 32.15 | 960 | 456.0
Old Result: T=24.23*C, H=32.15%, p=959.66hPa, a=456.04m
--------------------------------------------
Measurement started!
New Result: {"T":24.2,"H":32.2,"p":959.6,"a":456.5}
Old Result: 24.24 | 32.16 | 960 | 456.5
Old Result: T=24.24*C, H=32.16%, p=959.61hPa, a=456.52m
--------------------------------------------
```

![BME280 I2C Adapter](./images/D1_BME280Adapter1.png "BME280 I2C Adapter")    
_Fig. 1: D1 mini with I2C adapter for BME280_   

