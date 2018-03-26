# D1 mini mit BME280: Anzeige von Temperatur, Feuchtigkeit, Luftdruck und H&ouml;he &uuml;ber die RS-232
Sketch: D1_oop34_BME280.ino, Version 2018-03-26   
[English Version](./README.md "English Version")   

Dieses einfache Beispiel misst Temperatur, Feuchtigkeit Luftdruck und H&ouml;he unter Verwendung des Sensors BME280 und zeigt die Werte &uuml;ber die serielle Schnittstelle (RS-232, 115200 Baud) an.   
   
| ___BME280___  | Wertebereich          | Toleranz     |
|:------------- |:---------------------:|:------------ |
| Temperatur    | -40&deg;C...+85&deg;C | &#x00B1;1&deg;C (0,01&deg;C Aufl&ouml;sung) |
| Feuchtigkeit  | 0%...100%             | &#x00B1;3%RH |
| Luftdruck     | 300...1100hPa         | &#x00B1;1,0hPa |   
   
## Hardware
* WeMos D1 Mini
* BME280 am I2C-Bus: SCL=D1, SDA=D2, GND, 3V3 (I2C-Adresse 0x76)

**Beispiel f&uuml;r die Ausgabe auf der seriellen Schnittstelle:**
```
BME280 found!
{"T":21.9,"H":0.0,"p":657.9,"a":3497.0}
21.89 | 0.01 | 658 | 3497.0
T=21.89*C, H=0.01%, p=657.95hPa, a=3496.99m
--------------------------------------------
{"T":27.7,"H":23.2,"p":972.3,"a":347.0}
27.73 | 23.17 | 972 | 347.0
T=27.73*C, H=23.17%, p=972.27hPa, a=346.96m
--------------------------------------------
```
