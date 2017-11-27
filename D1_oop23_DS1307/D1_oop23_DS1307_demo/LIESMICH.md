# D1 mini: Beispiel zur Verwendung der Klasse DS1307
Sketch: D1_oop23_DS1307_demo, Version 2017-11-27   
[English Version](./README.md "English Version")

Dieses Beispielprogramm zeigt die Verwendung der Klasse **`DS1307`** zur Kommunikation mit der Echtzeituhr (Real Time Clock, RTC) DS1307 &uuml;ber den I2C-Bus. Die Klasse dient nur zum Datenaustausch zwischen einem Programm und der DS1307 Uhr und ber&uuml;cksichtigt nicht die Sommerzeit, etc.     

**Programmfunktion im Detail**   
Vorgabewert f&uuml;r die I2C-7bit-Adresse ist 0x68.   
Alle Ergebnisse werden auf die serielle Schnittstelle ausgegeben (115200 Baud).
* Abfrage der I2C-Adresse des DS1307 und Anzeige des Ergebnisses
* Setzen von Datum und Uhrzeit auf einen Wert kurz vor Mitternacht und den kritischen Wochentagwechsel von Samstag (6) auf Sonntag (0)
* L&ouml;schen/Beschreiben des RAMs mit dem Vorgabewert (0) und Anzeige, ob dies erfolgreich war
* Schreiben eines Bytes und einer ganzen Zahl (int) ins RAM, zur&uuml;cklesen und Kontrolle, ob alles passt
* Hole Wochentag, Datum, Uhrzeit und Sekunden seit 1.1.1970 vom DS1307 und zeige die Werte alle zwei Sekunden an (insgesamt f&uuml;nfmal)
* Liefere die zuletzt ausgelesenen Werte (ohne DS1307-Zugriff) und zeige sie an (nur zur Kontrolle)   

M&ouml;chte man die Funktion der Batterie des Datenlogger Shields testen:
* Zeilen f&uuml;r das Setzen von Datum und Uhrzeit auskommentieren
* Programm neu kompilieren und in den D1 mini laden
* D1 mini + Datenlogger Shield von der Versorgungsspannung trennen
* wieder einschalten und die Werte von der Uhr einlesen!   


## Hardware
* WeMos D1 mini
* Datenlogger Shield (microSD + DS1307RTC Shield)   
  DS1307  verwendet I2C (D1-SCL, D2-SDA)   
  microSD verwendet SPI (D5-CLK, D6-MISO, D7-MOSI, D8-CS)   

![Bild: D1mini microSD + DS1307 shield](./images/D1_microSD_DS1307_shield.png "D1mini microSD + DS1307 shield")

**Beispiel f&uuml;r die Ausgabe auf der seriellen Schnittstelle:**
```
class DS1307 demo:
i2c-status for 0x68 is OK
Winterzeit!
RAM clearing ok.
RAM set byte ok.
RAM set int16 ok.
-----Data from DS1307----------------------
SA 25.11.2017 23:59:56 = 20171125 235956 = 1511654396
Last values (should be the same):
SA 25.11.2017 23:59:56 = 20171125 235956 = 1511654396
...
-----Data from DS1307----------------------
SO 26.11.2017 00:00:04 = 20171126 000004 = 1511654404
Last values (should be the same):
SO 26.11.2017 00:00:04 = 20171126 000004 = 1511654404
Finished :)
```
