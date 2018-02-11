# D1 mini: display temperature and humidity on oled
Sketch: D1_oop30_SHT30_oled.ino, Version 2018-02-10   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

This example measures temperature and humidity using a SHT30 sensor and shows the values on an oled shield.   
| ___SHT30___ | Range | Tolerance |
|:--------- |:-----:|:--------- |   
| Temperature | -40&deg;C...+125&deg;C | &#x00B1;0,7% (0&deg;..+65&deg;C: +-0,2&deg;C) |   
| Humidity    | 0%...100% | &#x00B1;2%RH |   

## Hardware
* WeMos D1 mini
* SHT30 Shield (I2C-address 0x45)
* OLED Shield  (SSD1306, 64x48 pixel, I2C-address 0x3D)
* (Dual Base Shield by request)

![D1mini SHT30 shield](./images/D1_SHT30_dual.png "D1mini with SHT30 and OLED shield")
