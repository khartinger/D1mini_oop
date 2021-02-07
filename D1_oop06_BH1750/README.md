# D1 mini with BH1750: Displaying the brightness via the serial interface
Sketch: D1_oop06_HB1750.ino, Version 2021-02-07   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   


Using a BH1750, the brightness is measured and displayed via the serial interface.   

| ___BH1750___ | Value range | Tolerance |
|:------------- |:---------------------:|:------------------------ |
| Brightness | 0 lx ... 65535 lx | 1 lx or &#x00B1;20% |
   
__Note__   
The first values read in are often incorrect.   

## Hardware
1. WeMos D1 mini (OR ESP32 D1 mini)
2. brightness sensor BH1750 on I2C bus: SCL=D1, SDA=D2, GND, 3V3 (I2C-address 0x23 or 0x5C)

**Example of the output on the serial interface
```
setup(): --Start--
BH1750 found! I2C address is 0x23
setup(): --Finished--

1:
2: 
3: measurement started! 
4:
5: Brightness: 193 lx
6: 
7: measurement started! 
8: 
9: Brightness: 193 lx
10: 
```

## Sensor

![BH1750](./images/BH1750.png "BH1750")    
_image 1: Sensor BH1750_   
