# D1 mini: Einfache Textanzeige auf OLED Display screen4
Sketch: D1_oop03_oled_screen4 _V2.ino, Version 2017-12-31   
[English version](./README.md "English version")   

Dieses Beispielprogramm schreibt Text auf ein OLED Shield unter Verwendung der Klasse __Screen1__. Es wird folgendes angezeigt:   
* "Temperatur"  20,0 bis 40,0&deg;C (in 0,1 &deg; Schritten pro Sekunde) und
* "humidity"    Doppelter Wert der Temperatur

Die Klasse __Screen1__ basiert auf Adafruit Code, der sich in der Datei `D1_class_Screen_64x48.*` befindet.

## Hardware
* WeMos D1 mini
* OLED Shield: SSD1306, 64x48 pixel, I2C

![D1mini screen4](./images/D1_oop3_oled_screen4.png "D1mini screen4")