# D1 mini: Demo zum Betrieb zweier BME280   
Version 2018-11-02   
[English version](./README.md "English version")   

Dieses sehr einfache Beispiel zeigt den Betrieb von zwei Temperatur/Feuchtigkeit/Luftdruck-Sensoren BME280 bzw. BMP280 am D1 mini.   
Zu Beginn wird der Sensortyp bestimmt und danach werden alle 2 Sekunden die Messwerte im JSON-Format auf der seriellen Schnittstelle (115200Bd) angezeigt.

### Hardware
(1) WeMos D1 mini   
    2x BME280 am I2C-Bus: SCL=D1, SDA=D2, GND, 3V3 (I2C-Adresse 0x76 und 0x77)

### Software
Verwendet die Klasse `BME280` (siehe Verzeichnis `src\bme280`)  

__Ausgabe__   
```
Sensor type at address 0x76: BME280
  Status: OK
Sensor type at address 0x77: BMP280 (no humidity value!)
  Status: OK
Sensor 0x76: {"T":22.0,"H":47.3,"p":980.1,"a":279.3}
Sensor 0x77: {"T":26.0,"H":0.0,"p":714.2,"a":2854.4}
--------------------------------------------
Sensor 0x76: {"T":22.0,"H":47.4,"p":980.2,"a":279.0}
Sensor 0x77: {"T":21.8,"H":0.0,"p":979.5,"a":284.9}
--------------------------------------------
```
