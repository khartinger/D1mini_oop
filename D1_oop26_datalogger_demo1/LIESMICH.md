# D1 mini - Demo: Data logger shield
Sketch: D1_oop26_datalogger_demo1.ino, Version 2017-12-03   
[English Version](./README.md "English Version")   

Dieses Beispielprogramm liest 5 Analogwerte ein und schreibt sie mit Datum und Uhrzeit in die Datei "analogin.txt" aud der microSD Karte. Status und Eegebnisse werden &uuml;ber die serielle Schnittstelle angezeigt (115200 Baud).     
* Setup: Initialisieren der seriellen Schnittstelle, einschalten der blauen LED und setzen von Referenzpunkten f&uuml;r die A/D-Wandlung.
* Setup: L&ouml;schen der alten Datei auf der micro SD Karte.
* Setup: Versuch, sich mit dem WLAN zu verbinden, die aktuelle Zeit von einem Server zu holen und in die RTC (Uhr) zu schreiben. Gelingt dies nicht, wird die aktuelle Zeit der RTC verwendet.
* Loop : Einlesen des Analogwertes, Anzeige und Schreiben in die Datei.
* Ende : Blaue LED abschalten.
* Ende : Dateiinhalt &uuml;ber die serielle Schnittstelle anzeigen.

__*Nicht vergessen: Die WLAN-Daten an das eigene Netzwerk anpassen*__ in der Zeile:   
`TimeHttp time1("..ssid..", "..password..","httpservername");`  

## Hardware
* WeMos D1 mini
* Datenlogger Shield (microSD + DS1307RTC Shield)   
  DS1307  verwendet I2C (D1-SCL, D2-SDA)   
  microSD verwendet SPI (D5-CLK, D6-MISO, D7-MOSI, D8-CS)   
* Poti 10kOhm zwischen 3V3 und 0V

![Bild: D1mini microSD + DS1307 shield + Poti](./images/D1_datalogger_poti_171203.png "D1mini microSD + DS1307 shield + Poti")

## Software
Folgende Klassen werden f&uuml;r dieses Programm ben&ouml;tigt:   
* Dout    : Ein- und Ausschalten der blauen LED
* Ain     : Einlesen des Analogwertes
* DS1307  : Uhr (RTC = Real Time Clock)
* TimeHttp: Holen von Datum und Uhrzeit von einem Server
* SDfile  : Dateibearbeitung auf der micro SD Karte

**Beispiel f&uuml;r die Ausgabe auf der seriellen Schnittstelle:**
```
Initializing SD card.....Ready :)

09826c6d

Starting a HTTP-request...
Disconnected. Try to reconnect...

Connecting to Raspi11
...........
Connected! IP address is 192.168.1.169
Connected to Wifi!
Connected to server!
DS1307 set time: OK
1: 1.49 V
2: 1.10 V
3: 0.87 V
4: 0.45 V
5: 0.08 V

--Measurement finished - File content:--
03.12.2017 18:33:28 1.49 V
03.12.2017 18:33:30 1.10 V
03.12.2017 18:33:32 0.87 V
03.12.2017 18:33:34 0.45 V
03.12.2017 18:33:36 0.08 V
```
