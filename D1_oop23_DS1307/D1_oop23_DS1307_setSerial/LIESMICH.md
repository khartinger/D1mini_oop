# D1 mini: Setzen von Datum und Uhrzeit bei RTC DS1307 &uuml;ber die serielle Schnittstelle
Sketch: D1_oop23_DS1307_setSerial, Version 2017-11-27   
[English Version](./README.md "English Version")

Setze Datum und Uhrzeit bei einer Echtzeituhr DS1307 (RTC, Real Time Clock) unter Verwendung der Klasse **`DS1307`**.
* Senden einer Zeichenkette `T_yyyymmdd HHMMSS` (gefolgt von einem new-line-Zeichen \n) &uuml;ber die serielle Schnittstelle setzt Datum und Uhrzeit auf die gegebenen Werte yyyy (Jahr) mm (Monat) dd (Tag) HH (Stunden) MM (Minuten) and SS (Sekunden).   
Beispiel: T_20171127 185040 setzt die DS1307-Register auf 27.11.2017 18:50:40 Uhr.   
* Datum und Uhrzeit werden alle 2 Sekunden angezeigt.

Bei Verwendung der Arduino-IDE zum Senden des Zeit-Strings: Im Drop-Down-Men&uuml; rechts unten im seriellen Monitor muss "Neue Zeile" eingestellt werden!   

Vorgabewert f&uuml;r die I2C-7bit-Adresse ist 0x68.   
Alle Ergebnisse werden auf die serielle Schnittstelle ausgegeben (115200 Baud).

## Hardware
* WeMos D1 mini
* Datenlogger Shield (microSD + DS1307RTC Shield)   
  DS1307  verwendet I2C (D1-SCL, D2-SDA)   
  microSD verwendet SPI (D5-CLK, D6-MISO, D7-MOSI, D8-CS)   

![Bild: D1mini microSD + DS1307 shield](./images/D1_microSD_DS1307_shield.png "D1mini microSD + DS1307 shield")

**Beispiel f&uuml;r die Ausgabe auf der seriellen Schnittstelle:**
```
RTS DS1307: OK
Arduino-IDE: Don't forget to set drop down 
menu on lower right to "New line" :-)
SO 26.11.2017 03:33:50  | Set time by sending T_yyyymmdd HHMMSS
SO 26.11.2017 03:33:52  | Set time by sending T_yyyymmdd HHMMSS
Set date: OK
Set time: OK
MO 27.11.2017 18:50:40  | Set time by sending T_yyyymmdd HHMMSS
MO 27.11.2017 18:50:42  | Set time by sending T_yyyymmdd HHMMSS
```
