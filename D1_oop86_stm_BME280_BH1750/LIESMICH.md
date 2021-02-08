# D1 mini: Verwendung der Sensoren BME280 und BH1750 in einer Statemachine
Sketch: D1_oop86_stm_BME280_BH1750.ino, Version 2021-02-08   
[--> English Version](./README.md "English Version")   

Dieses Beispielprogramm soll zeigen, dass die Klassen f&uuml;r den Temperatur-/Feuchte-/Luftdruck-Sensor BME280 sowie den Helligkeitssensor BH1750 den Ablauf einer Statemachine nicht blockieren.   
Zu diesem Zweck l&auml;uft die Statemachine im 0,1s-Takt und die Arbeitsdauer f&uuml;r jeden Schritt wird angezeigt (meist 0ms bis 3ms). Der Messbeginn jeder Messung sowie die Ergebnisse der Messung werden ebenfalls &uuml;ber die serielle Schnittstelle (115200Bd) ausgegeben (siehe Beispiel unten).

## Hardware
1. WeMos D1 Mini oder ESP32 D1 mini
2. BME280 Temperatursensor I2C-Bus (I2C-address 0x76 or 0x77)
3. BH1750 Helligkeitssensor am I2C-Bus (0x23, 0x5C)

**Beispiel f&uuml;r die Ausgabe auf der seriellen Schnittstelle:**
```
setup(): --Start--
BME280 found! I2C address is 0x76
BH1750 found! I2C address is 0x23
setup(): --Finished--

    1:  | 0ms
    2: BH1750 measurement started!  | 1ms
    3:  | 0ms
    4:  Brightness: 64 lx | 0ms
    5: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.5} | 2ms
    6: BH1750 measurement started!  | 1ms
    7:  | 0ms
    8:  Brightness: 71 lx | 1ms
    9:  | 0ms
   10: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.5}BH1750 measurement started!  | 2ms
   11:  | 0ms
   12:  Brightness: 71 lx | 0ms
   13:  | 0ms
   14: BH1750 measurement started!  | 1ms
   15: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.2} | 2ms
   16:  Brightness: 71 lx | 1ms
   17:  | 0ms
   18:  | 0ms
   19: BH1750 measurement started!  | 0ms
   20: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.8} | 2ms
   21:  Brightness: 71 lx | 0ms
   22:  | 1ms
   23: BH1750 measurement started!  | 0ms
   24:  | 0ms
   25: BME280 measurement started! {"T":24.2,"H":32.8,"p":957.3,"a":476.8} Brightness: 71 lx | 3ms
   26:  | 0ms
   27:  | 0ms
```
