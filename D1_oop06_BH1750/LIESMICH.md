# D1 mini mit BH1750: Anzeige der Helligkeit über die serielle Schnittstelle
Sketch: D1_oop06_HB1750.ino, Version 2021-02-07   
[--> English Version](./README.md "English Version")   

Mit Hilfe eines BH1750 wird die Helligkeit gemessen und über die serielle Schnittstelle angezeigt.   

| ___BH1750___  | Wertebereich          | Toleranz                 |
|:------------- |:---------------------:|:------------------------ |
| Helligkeit    |   0 lx ... 65535 lx   | 1 lx bzw.  &#x00B1;20% |
   
__Anmerkung__   
Die ersten eingelesenen Werte sind oft fehlerhaft.   

## Hardware
1. WeMos D1 mini (OR ESP32 D1 mini)
2. Helligkeitssensor BH1750 am I2C-Bus: SCL=D1, SDA=D2, GND, 3V3 (I2C-address 0x23 or 0x5C)

**Beispiel f&uuml;r die Ausgabe auf der seriellen Schnittstelle:**
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
_Bild 1: Sensor BH1750_   
