# D1 mini mit BH1750: Anzeige der Helligkeit auf OLED 0,66"
Sketch: D1_oop07_HB1750_oled.ino, Version 2021-02-07   
[--> English Version](./README.md "English Version")   

Mit Hilfe eines BH1750 wird die Helligkeit gemessen und auf einem OLED 0,66" Display angezeigt.   

| ___BH1750___  | Wertebereich          | Toleranz                 |
|:------------- |:---------------------:|:------------------------ |
| Helligkeit    |   0 lx ... 65535 lx   | 1 lx bzw.  &#x00B1;20% |
   
__Anmerkung__   
Die ersten eingelesenen Werte sind oft fehlerhaft.   

## Hardware
1. WeMos D1 mini (OR ESP32 D1 mini)
2. Helligkeitssensor BH1750 am I2C-Bus: SCL=D1, SDA=D2, GND, 3V3 (I2C-address 0x23 or 0x5C)
3. OLED Shield: 0.66", SSD1306, 64x48 pixel, I2C @ 0x3C

## Beispiele f&uuml;r Anzeigen
![D1mini, BH1750 und OLED](./images/D1_bh1750ok.png "D1mini, BH1750 und OLED") ![D1mini, BH1750 und OLED](./images/D1_bh1750noic.png "D1mini, BH1750 und OLED")    
_Bild 1: OLED Anzeige mit Messwert und bei einem Fehler (zB kein Sensor)_   
   
![BH1750](./images/BH1750.png "BH1750")    
_Bild 2: Sensor BH1750_   
