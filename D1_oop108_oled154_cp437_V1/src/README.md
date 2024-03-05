# D1 mini: Display of ASCII characters 32-255 on a 1.54" OLED display (code page 437)
Sketch: D1_oop02_oled154_cp437_V1.ino, Version 2024-02-05   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

This program displays all ASCII characters of the font `u8g2_font_KH_cp437_6x8_mf` on a 1.54 inch (or 2.4") OLED display with driver SD 1309.   
The characters correspond to code page CP437 and are 6x8 pixels in size (width 6, height 8 ).   
The font is contained in the class __Screen154__ (file `D1_class_Screen154.h`) and must also be declared in the file `u8g2.h`:   
(`external const uint8_t u8g2_font_KH_cp437_6x8_mf[] U8G2_FONT_SECTION("u8g2_font_KH_cp437_6x8_mf");`)   

![ASCII 032-111](./images/sd1309_ascii_cp437_032-111_v1.png "ASCII 032-111")   
![ASCII 112-191](./images/sd1309_ascii_cp437_112-191_v1.png "ASCII 112-191")   
![ASCII 192-255](./images/sd1309_ascii_cp437_192-255_v1.png "ASCII 192-255")   

## Changing individual characters
To change a character, the file `font_KH_cp437_6x8_mf.fon` must be loaded into the programme `Fony.exe`. After changing the characters, save the fon file ([x] Monospaced) and export it as a BDF file ([ ] Don't trim glyph bitmaps).   
The `.bdf` file can be converted into a c file containing the code for the character set using the command line programme `bdfconv.exe`:   
`bdfconv -f 1 -m "32-255" -n u8g2_font_KH_cp437_6x8_mf -o font_KH_cp437.c font_KH_cp437_6x8_mf.bdf`   
See also:   
* [https://github.com/olikraus/u8g2/blob/c7dc077995ca513337a7d2a1970df693e18ee308/doc/faq.txt#L245](https://github.com/olikraus/u8g2/blob/c7dc077995ca513337a7d2a1970df693e18ee308/doc/faq.txt#L245)   
* [https://forum.arduino.cc/t/u8g2-custom-font-tutorial-with-fony/686061](https://forum.arduino.cc/t/u8g2-custom-font-tutorial-with-fony/686061)   

## Hardware
* WeMos D1 mini or ESP32 D1 mini
* OLED: 1,54" or 2,4", SSD1309, 128x64 pixel, I2C
