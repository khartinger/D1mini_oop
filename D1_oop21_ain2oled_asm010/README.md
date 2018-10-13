# D1_oop21_ain2oled_asm010: Analog in to OLED

This example uses the classes __Ain__ and __Screen1__ to get analog input values and convert them to a current (1000 = 500mA).
The voltage is measured 16 times every second and the average is displayed.

## Hardware
* Wemos D1 mini
* OLED Shield: SSD1306, 64x48 pixel, I2C
* Current transformer ASM-010 and INA122 amplifier

![D1mini with D1 OLED Shield](./images/D1_oop21_ain2oled.png "Analog value on  OLED Shield")
