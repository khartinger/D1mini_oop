# D1_oop02_oled1_testall 
This example shows writing text on different screens on OLED shield using the class __Display0KH__ . The class extends the class Adafruit_SSD1306, so you can use all commands from this class as well.
The used Adafruit files are in the directory /libs, so you need not search for the zip files.

The 5x8 pixel font is located in /fonts. You can modify every character by using the OpenOffice Calc file myfont5x8.ods.

| files in /libs       | Size         |
| -------------------- | ------------ |
| Adafruit_GFX.cpp     | 32.436 Bytes |
| Adafruit_GFX.h       |  4.944 Bytes |
| Adafruit_SSD1306.cpp | 27.813 Bytes |
| Adafruit_SSD1306.h   |  5.806 Bytes |
| gfxfont.h            |    924 Bytes |
| glcdfont.c           |  8.486 Bytes |

## Hardware
* WeMos D1 mini
* OLED Shield: SSD1306, 64x48 pixel, I2C

![D1mini screen6](./images/D1_oled_screen6.png "D1mini screen6")
![D1mini screen6i](./images/D1_oled_screen6i.png "D1mini screen6i")
![D1mini screen5](./images/D1_oled_screen5.png "D1mini screen5")
![D1mini screen5i](./images/D1_oled_screen5i.png "D1mini screen5i")
![D1mini screen4](./images/D1_oled_screen4.png "D1mini screen4")
![D1mini screen4i](./images/D1_oled_screen4i.png "D1mini screen4i")
![D1mini screen4ii](./images/D1_oled_screen4ii.png "D1mini screen4ii")
![D1mini screen221](./images/D1_oled_screen221.png "D1mini screen221")
![D1mini screen112](./images/D1_oled_screen112.png "D1mini screen112")
![D1mini screen13](./images/D1_oled_screen13.png "D1mini screen13")
![D1mini screen12](./images/D1_oled_screen12.png "D1mini screen12")
![D1mini screen2](./images/D1_oled_screen2.png "D1mini screen2")

## Class diagram
Parameters
* line_ .... line on the oled (1 to max. line 6)
* text_ .... text to write (max. 10 chars or 5 big chars)
* align_ ... align of text (default: 'l'=left. 'c'=center, 'r'=right, 'L'=left+overwrite, 'C'=center+overwrite, 'R'=right+overwrite)
* cls_ ..... clear display (default: false = do not clear display)

screen names (examples)
* screen13 .... 1 line with big chars, 3 lines with normal chars
* screen4 ..... 4 lines with normal chars and border arround
* screen4i .... 4 lines with normal chars, line 1 inverted
* screen4ii ... 4 lines with normal chars, line 1 and 3 inverted

| class Display0KH (extends Adafruit_SSD1306) |
| ------------------------------------------- |
| + Display0KH() <br> + void screen6 (int line_, String text_, char align_, bool cls_) <br> + void screen6i(int line_, String text_, char align_, bool cls_) <br> + void screen5 (int line_, String text_, char align_, bool cls_) <br> + void screen5i(int line_, String text_, char align_, bool cls_) <br> +  void screen4 (int line_, String text_, char align_, bool cls_) <br> + void screen4i(int line_, String text_, char align_, bool cls_) <br> + void screen4ii(int line_, String text_, char align_, bool cls_) <br> + void screen221(int line_, String text_, char align_, bool cls_) |
| _screens with big character-lines_ |
| + void screen112(int line_, String text_, char align_, bool cls_) <br> + void screen13(int line_, String text_, char align_, bool cls_) <br> + void screen12(int line_, String text_, char align_, bool cls_) <br> + void screen2 (int line_, String text_, char align_, bool cls_) |
| - String mytrim(int max_, String txt_, char align) <br> - void scr6(int line_, String text_, char align_, bool cls_, bool invert_) <br> - void scr5(int line_, String text_, char align_, bool cls_, bool invert_) <br> - void scr4(int line_, String text_, char align_, bool cls_, bool invert_) |
