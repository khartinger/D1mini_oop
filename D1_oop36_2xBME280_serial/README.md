# D1 mini: Example for using zwo BME280   
Version 2018-11-02   
[German version](./LIESMICH.md "German version")   

This is a very simple example for using zwo BME280 or BMP280 connected to a D1 mini.   
On start the program checks the type of sensor and then it prints the measure values to Serial (115200Bd) using a JSON format.

### Hardware
(1) WeMos D1 mini   
    2xBME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3 (I2C-address 0x76 and 0x77)

### Software
Uses class `BME280` (see directory `src\bme280`)   

__Serial Output__
```
Sensor type at address 0x76: BME280
  Status: OK
Sensor type at address 0x77: BMP280 (no humidity value!)
  Status: OK
Sensor 0x76: {"T":22.0,"H":47.3,"p":980.1,"a":279.3}
Sensor 0x77: {"T":26.0,"H":0.0,"p":714.2,"a":2854.4}
--------------------------------------------
Sensor 0x76: {"T":22.0,"H":47.4,"p":980.2,"a":279.0}
Sensor 0x77: {"T":21.8,"H":0.0,"p":979.5,"a":284.9}
--------------------------------------------
```
