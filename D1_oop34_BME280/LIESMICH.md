# D1 mini mit BME280: Anzeige von Temperatur, Feuchtigkeit, Luftdruck und H&ouml;he &uuml;ber die RS-232
Sketch: D1_oop34_BME280.ino, Version 2021-02-01   
[--> English Version](./README.md "English Version")   

Dieses Beispielprogramm zeigt das Messen von Temperatur, Feuchtigkeit, Luftdruck und H&ouml;he mit Hilfe des Sensors BME280. Die Werte werden im Abstand von 2 Sekunden gemessen und &uuml;ber die serielle Schnittstelle (RS-232, 115200 Baud) angezeigt.   
Verwendet man statt des BME280 einen Sensor BMP280, so ist der Feuchtigkeitswert immer 0, da dieser Sensor keine Luftfeuchtigkeit messen kann ;) 
   
| ___BME280___  | Wertebereich          | Toleranz     |
|:------------- |:---------------------:|:------------ |
| Temperatur    | -40&deg;C...+85&deg;C | &#x00B1;1&deg;C (0,01&deg;C Aufl&ouml;sung) |
| Feuchtigkeit  | 0%...100%             | &#x00B1;3%RH |
| Luftdruck     | 300...1100hPa         | &#x00B1;1,0hPa |   
   
__Anmerkung__   

Der erste eingelesene Wert ist meist fehlerhaft und sollte nicht verwendet werden ;)   

## Hardware
* WeMos D1 Mini
* BME280 am I2C-Bus: SCL=D1, SDA=D2, GND, 3V3 (I2C-Adresse 0x76 oder 0x77)

**Beispiel f&uuml;r die Ausgabe auf der seriellen Schnittstelle:**
```
setup(): --Start--
BME280 found!        
I2C address: 0x77    
setup(): --Finished--

{"T":24.4,"H":0.0,"p":611.7,"a":4059.8}
24.35 | 0.01 | 612 | 4059.8
T=24.35*C, H=0.01%, p=611.66hPa, a=4059.84m
--------------------------------------------
{"T":22.5,"H":28.3,"p":960.6,"a":447.5}
22.53 | 28.28 | 961 | 447.5
T=22.53*C, H=28.28%, p=960.64hPa, a=447.49m
--------------------------------------------
{"T":22.5,"H":28.3,"p":960.6,"a":448.2}
22.51 | 28.27 | 961 | 448.2
T=22.51*C, H=28.27%, p=960.57hPa, a=448.17m
```

![BME280 I2C Adapter](./images/D1_BME280Adapter1.png "BME280 I2C Adapter")    
_Bild 1: D1 mini mit I2C-Adapter f&uuml;r BME280_   
