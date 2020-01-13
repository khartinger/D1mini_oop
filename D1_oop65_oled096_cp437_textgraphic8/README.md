# D1 mini: Show 8 px text graphic characters of codepage CP437 on an oled 0,96"
Version 2020-01-08 D1_oop65_oled096_cp437_textgraphic8.ino   
[German version](./LIESMICH.md "German version")   

Just for fun: This example draws borders using the built in codepage CP437 font `fontKH_cp437_8x6` (8 pixel high) on an oled 0,96" (128x64 pixel).   
Special feature of built fonts: character 0x80 = 128 = &#x20AC; (= Euro, instead of &#xc7;)

![D1mini CP437 text graphic on OLED 0.96"](./images/D1_oled096_textgraphic8.png "CP437 text graphic on OLED 0.96\"")      
_Figure 1: Some borders made of text graphic characters_   

### Hardware
(1) WeMos D1 mini   
(2) One-color OLED 0.96" mounted on a proto shield (128x64 pixel, I2C-addr 0x3C)   

### Software
Uses files (classes) `D1_class_Screen096`, `OLEDDisplay`, `SSD1306Wire` and `OLEDDisplayFonts.h` (see directory `src\screen096`)   
