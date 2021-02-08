# Using the BME280 and BH1750 sensors in a Statemachine
Sketch: D1_oop86_stm_BME280_BH1750.ino, Version 2021-02-08   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   

This sample programme is intended to show that the classes for the temperature/humidity/air pressure sensor BME280 and the brightness sensor BH1750 do not block the running of a statemachine.   
For this purpose, the statemachine runs in 0.1s intervals and the working time for each step is displayed (usually 0ms to 3ms). The start of each measurement and the results of the measurement are also output via the serial interface (115200Bd) (see example below).

## Hardware
1. WeMos D1 Mini or ESP32 D1 mini
2. BME280 temperature sensor I2C bus (I2C address 0x76 or 0x77)
3. BH1750 brightness sensor on I2C bus (0x23, 0x5C)

**Example of the output on the serial interface**
```
setup(): --Start--
BME280 found! I2C address is 0x76
BH1750 found! I2C address is 0x23
setup(): --Finished--

    1:  | 0ms
    2: BH1750 measurement started!  | 1ms
    3:  | 0ms
    4:  Brightness: 64 lx | 0ms
    5: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.5} | 2ms
    6: BH1750 measurement started!  | 1ms
    7:  | 0ms
    8:  Brightness: 71 lx | 1ms
    9:  | 0ms
   10: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.5}BH1750 measurement started!  | 2ms
   11:  | 0ms
   12:  Brightness: 71 lx | 0ms
   13:  | 0ms
   14: BH1750 measurement started!  | 1ms
   15: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.2} | 2ms
   16:  Brightness: 71 lx | 1ms
   17:  | 0ms
   18:  | 0ms
   19: BH1750 measurement started!  | 0ms
   20: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.8} | 2ms
   21:  Brightness: 71 lx | 0ms
   22:  | 1ms
   23: BH1750 measurement started!  | 0ms
   24:  | 0ms
   25: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.8} Brightness: 71 lx | 3ms
   26:  | 0ms
   27:  | 0ms
```
