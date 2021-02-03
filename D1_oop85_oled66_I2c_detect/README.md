# D1 mini: Display of I2C addresses
Sketch: D1_oop85_oled66_I2c_detect.ino, Version 2021-02-02   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   

This programme for a D1 mini (or ESP32 D1mini) scans the I2C bus from address 0x03 to 0x77 every 5 seconds and shows the addresses found on the 0.66" display and via the serial interface (115200Bd).

__Output of the programme__
```
setup(): --Start--
I2C, display OK
setup(): --Finished--

Scanning i2c 7-bit-addresses 0x03 to 0x77...
23 3C 76 
Scanning i2c 7-bit-addresses 0x03 to 0x77...
23 3C 76 
```

### Hardware 
1. WeMos D1 mini or ESP32 D1 mini   
2. Adapter for I2C bus
3. OLED Shield: 0.66", SSD1306, 64x48 pixel, I2C @ 0x3C
4. More I2C sensors (0x3C is the OLED shield)

![I2C address search](./images/D1_i2c_detect1.png "I2C address search") ![I2C address search2](./images/D1_i2c_detect2.png "I2C address search2")   
_Fig. 1: Display of the found I2C addresses_   

