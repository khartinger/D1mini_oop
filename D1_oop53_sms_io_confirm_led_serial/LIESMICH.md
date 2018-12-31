# D1 mini: Schalten der blauen Led mit SMS und Best&auml;tigung
Version 2018-12-30, Datei: `D1_oop53_sms_io_confirm_led_serial.ino`   
[English version](./README.md "English version")   

Empf&auml;ngt der D1 mini eine SMS mit dem Inhalt `#led on#` oder `#led off#`, so schaltet er die blaue LED entsprechend und sendet eine Best&auml;tigungs-SMS an den Sender zur&uuml;ck. SMS mit einem anderen Inhalt werden mit "__Received: &lt;Inhalt&gt;__" beantwortet.   
Eintreffende SMS werden immer an der ersten freien Stelle im Speicher abgelegt. L&ouml;scht man beim Systemstart oder nach der Ausf&uuml;hrung eines SMS-Kommandos die erste SMS, werden weitere eintreffende SMS immer an der gleichen Stelle abgelegt, was das Auffinden einer neuen SMS erheblich vereinfacht. Die Nummer der ersten Speicherstelle ist f&uuml;r den SIM-Karten-Speicher "SM" 1 und f&uuml;r das Modem "ME" 21 (Vorgabewert).  
* SIM-Ansteuerung  : Software Serial @ D5 (=TX), D6 (=RX)   
* Status Info am PC: Hardware Serial @ TX, RX   

* Klasse __`GSMmodule`__: Die Einstellung der Schnittstelle erfolgt beim Erstellen des Objektes. Ohne Parameter wird die Software-serielle Schnittstelle gew&auml;hlt, mit Parameter `false` wird die Hardware-Schnittstelle gew&auml;hlt. ZB   
`GSMmodule mod1;                        // SIM module soft serial`   
`//GSMmodule mod1(false);               // false=Hardware serial`   

__Anmerkung:__ Das Programm ist fast identisch mit `oop52_sms_receive_led_serial.ino`.   

## Hardware
1. WeMos D1 mini   
2. Battery Shield for WEMOS D1 mini mit kleinem Akku HM04S (3,7V 350mAh 1,3Wh)   
3. Selbstbau SIM800C 9pol ProtoBoard Shield mit angestecktem SIM800C GSM GPRS module   

![D1 SIM800C_oled1](./images/D1_SIM800C_oled1.png "D1mini mit SIM800C 9pol ProtoBoard Shield")   
_Bild 1: D1mini mit Battery Shield, 9pol ProtoBoard Shield und 2-Button OLED Shield._ 

Oder   
1. WeMos D1 mini   
2. Selbstbau SIM800C Adapter auf Lochraster f&uuml;r "SIM800C GSM GPRS module STM32"      
![D1 SIM800C_STM32a](./images/D1_sms_confirm1.png "D1mini mit SIM800C Lochraster-Shield")   
_Bild 2: D1mini mit &uuml;ber die Hardware-serielle Schnittstelle angeschlossenem SIM800C GSM GPRS module STM32_ 

## Software - Details
Nach dem Programmstart wird zuerst der LED-Ausgang initialisiert, und es wird durch Aufruf der Methode `begin()` in einer Schleife versucht, folgende Einstellungen vorzunehmen: Echo abschalten, SMS im Textformat, Modem-Speicher (ME) f&uuml;r SMS verwenden, die SMS-Nummer der ersten Speicherstelle im Speicher ermitteln und abfragen, in welchem Netz sich die SIM registriert hat.   
Danach wird die Speicherstelle der SMS Nummer 1 gel&ouml;scht und das Programm wartet in `loop()` auf das Eintreffen einer SMS.   
Eine eintreffende SMS wird gelesen, der Text auf ein Befehlskommando untersucht, der Befehl ausgef&uuml;hrt und das Ergebnis &uuml;ber die serielle Schnittstelle  angezeigt. Dieses Ergebnis wird als Text an den Sender der SMS zur&uuml;ckgesendet (gleiche Telefonnummer), das Sendeergebnis angezeigt und der SMS-Speicherplatz 1 gel&ouml;scht.

###__*Beispiel-Ausdruck von der seriellen Schnittstelle eines PCs*__
```
2018-12-30 D1_oop53_sms_io_confirm_led_serial.ino
Wait for SMS #led on# or #led off#, send answer SMS
-----Init Modem---
-----Modem OK-----
Delete location for command sms: #~OK#~
-----Ready--------
SMS received!
*** LED is on  ***
+CMGS: 126
OK

SMS received!
*** LED is off ***
SMS received!
*** LED is off ***
+CMGS: 128
OK

SMS received!
Received: Led on
+CMGS: 129
OK
```
## Besonderheiten des Hardware-Aufbaus  
* Die Stromversorgung erfolgt &uuml;ber das Battery Shield, da der Spannungsstabilisator auf dem D1 mini nicht gen&uuml;gend (Spitzen-)Strom f&uuml;r das SIM-Modul liefert.   
* Der Anschluss des SIM-Moduls erfolgt &uuml;ber ein Selbstbau SIM800C 9pol ProtoBoard Shield und eine Software-Serial-Schnittstelle (D5=TX, D6=RX)   
* Optional: Der Anschluss eines PCs (zur Information) erfolgt &uuml;ber die TX/RX-Pins und einen 3,3V-RS232-Adapter.   
W&auml;hrend der Programmierung/Testphase kann dies auch &uuml;ber den USB-Anschluss und die Arduino-Programmieroberfl&auml;che erfolgen.   

![D1_SIM800_shield_circuit](./images/D1_SIM800_shield_circuit1.png "D1mini mit D1_SIM800-Shield Schaltung")   
_Bild 3: Schaltung des D1 mini SIM800 Shields._ 

## Vorbereiten des SIM-Moduls
* Micro-SIM-Karte kaufen, registrieren und aufladen.   
F&uuml;r &Ouml;sterreich: Pre-Paid-SIM zB von Drei, HOT oder B.free.   
* Einlegen der Micro-SIM-Karte in das SIM-Modul, danach das Modul am Selbstbau SIM800C 9pol ProtoBoard Shield anstecken.   
* Ist das SIM-Modul angesteckt, so gibt die rote LED Hinweise zum Modul-Zustand:   
--> Blinkt sie in kurzen Abst&auml;nden, ist sie beim Booten oder beim Versuch, sich in das Provider-Netz einzuloggen.   
--> Blitzt sie kurz im Abstand von ca. 3 Sekunden auf, ist sie zum Senden und Empfangen von SMS bereit.   
* Kommt es w&auml;hrend des SMS-Sendevorgangs zum schnellen Blinken der LED, dann bootet das SIM-Modul neu. Das deutet darauf hin, dass die Stromversorgung zu schwach ist.

## Ansteuerung des SIM-Moduls
Die Software-Ansteuerung des SIM-Moduls erfolgt grunds&auml;tzlich durch Senden von AT-Befehlen. Zum &Uuml;berpr&uuml;fen des SIM-Moduls und zum Senden der SMS werden daher folgende drei AT-Befehle &uuml;ber die (Software-) serielle Schnittstelle gesendet:   
| Befehl                   | Bedeutung             |   
|--------------------------|-----------------------|   
| `ATE0`                   | Test, ob ein Modem angeschlossen ist und Abschalten des Zur&uuml;cksendens der empfangenen Befehle (Echo). Die Antwort sollte "OK" sein. |   
| `ATE1`                   | Schalte das Echo ein. |    
| `AT+CMGF=1`              | Umstellen des SMS-Formats auf Text. |    
| `AT+CPMS="ME","ME","ME"` | Define memory location for SMS (ME=Modul). |    
| `AT+CNMI=2,1,2,1,0`      | Info, wenn eine SMS empfangen wird. |    
| `AT+CMGS="0680xxxxxxx"`  | Senden einer SMS. Nach Eingabe des Befehls erscheint als Eingabeaufforderung ein Gr&ouml;&szlig;erzeichen &gt;, worauf der SMS-Text eingegeben werden kann. <br>__Wichtig__: Der SMS-Text muss mit &lt;Strg&gt;Z = 0x1A = 26 abgeschlossen werden! |       
### Einige weitere hilfreiche AT Kommandos
| Befehl                   | Bedeutung             |   
|--------------------------|-----------------------|   
| `AT+CPIN?`  | Test, ob eine SIM-Karte da ist und die PIN-Abfrage  nicht aktiviert ist. Die Antwort sollte sein: `+CPIN: READY` |    
| `AT+CGREG?` | Abfrage des Registrationsstatus des Modems. Antwort sollte sein: `+CGREG: 0,2`, wenn die SIM-Karte im Heim-Netzwerk des Providers registriert ist. |    
| `AT+COPS?`  | Abfrage der Network Information. Antwort sollte sein: `+COPS: 0,0,"one"`, wenn die SIM-Karte im Heim-Netzwerk des Providers registriert ist (oder "A1", "T-Mobile A" etc.). |    
| `AT+CSQ`    | Anzeige der Signalqualit&auml;t. Die Antwort k&ouml;nnte zB `+CSQ: 6,0` sein. <br>Werte f&uuml;r die Signalst&auml;rke: 0 = 113dBm oder weniger (kein Signal), 2..9 minimal, 10..14 OK, 15..19 gut, 20..30 ausgezeichnet, 31 = 51dBm oder gr&ouml;&szlig;er (besser), 99 Wert nicht bestimmbar/unbekannt. |    
| `ATD08006642903;` | Nummer f&uuml;r einen Sprachanruf w&auml;hlen (ZB Abfrage des Guthabens einer A1 prepaid SIM - Antwort kommt als SMS). <br>Nicht vergessen: den Anruf mit `ATH` beenden! |    
| `ATH`       | Beenden eines Sprachanrufs. |    
| `ATS0=1`    | Setzen der Anzahl an "RING"s vor dem automatischen Abheben eines eintreffenden Sprachanrufs. Nicht vergessen: den Anruf mit `ATH` beenden! Abschalten des automatischen Abhebens mit `ATS0=0`. |    
| `ATA`       | Abheben eines hereinkommenden Sprachanrufs. Nicht vergessen: den Anruf mit `ATH` beenden! |    
| `AT+CLCC`   | Anzeige, ob ein Sprachanruf aktiv ist. ("List Current Calls") |    
| `AT+COPS=0` | Netzwerk-Registrierung erzwingen. |    
| `AT+COPS=1,0,"A1"` | Manuelle Netzwerk-Registrierung im Netzwerk "A1". |    
| `AT+CCID`   | SIM-Kartennummer, zB 8943102101xxxxxxxxxf. |    
| `AT+CNUM`   | Sollte die eigene Telefonnummer anzeigen (ist nicht realisiert). |    

##Selbstbau SIM800C 9pol ProtoBoard Shield
Das SIM-Modul "SIM800C GSM GPRS module STM32" hat einige elektrische Besonderheiten:   
* Versorgungsspannung 3,4V bis 4,4V (NICHT 3,3V und NICHT 5V).   
Die Stromversorgung mit einem 3,7 V Lithium Akku ist laut H&auml;ndlerbeschreibung m&ouml;glich.
* Kurzzeitig sehr hoher Stromverbrauch beim SMS-Senden (bis zu 2A).
* Serieller Port Input: low -0,3..0,7V, high 2,1...3,1V   
Auch wenn in der H&auml;ndler-Beschreibung "Kompatibel mit 5V und 3V3 Systemen" steht, stimmt das nicht. Betr&auml;gt die Spannung an RX/TX 3,3V, so kommt es immer wieder zu Boot-Vorg&auml;ngen, speziell beim Versuch, eine SMS zu schicken.   
Auf der seriellen Schnittstelle wird dann pl&ouml;tzlich "Call Ready" und "SMS ready" ausgegeben und die rote LED blinkt schnell.   

Aus diesem Grund ist es erforderlich, eine Spannungsanpassungsschaltung zu erstellen (Voltage Interface Board). Der Aufbau kann auf einem ProtoBoard oder einer Lochrasterplatine erfolgen.   

![D1 9pol ProtoBoard Shield](./images/D1_SIM800C_STM32_Connect.png "D1mini SIM800C 9pol ProtoBoard Shield")   
_Bild 4: Schaltung des SIM800C 9pol ProtoBoard Shield_   

Die Erzeugung der 3,5V (3,3V) mit Hilfe der Zenerdiode dient nur zur Versorgung eines eventuell ben&ouml;tigten RS232-Pegelwandlers und kann auch weggelassen werden.   
Statt des USB-Anschlusses kann auch der 5V-Pin des D1 mini verwendet werden, wenn die Stromversorgung &uuml;ber ein Battery Shield erfolgt.

![D1 9pol SIM interface boards](./images/D1_SIM800C_interfaceboards_comp.png "D1mini SIM800C D1 9pol interface boards") ![D1 9pol SIM interface boards](./images/D1_SIM800C_interfaceboards_solder.png "D1mini SIM800C D1 9pol interface boards (solder side)")      
_Bild 5: Prototypen der SIM800-Interface Boards (Bauteil- und L&ouml;tseite)_   
Anmerkung: Bei der Bauteilplatzierung gibt es Optimierungspotenzial ;)

"SIM800C 9pol ProtoBoard Shield" (links im Bild): Oben ist der Anschluss f&uuml;r die serielle Hardware-Schnittstelle herausgef&uuml;hrt (RX-TX-GND-3V3).   
Lochrasterplatine (rechts im Bild): Die Stromversorgung des SIM-Moduls erfolgt &uuml;ber einen eigenen USB-Anschluss. Der Mikrocontroller (zB Raspberry Pi) ist &uuml;ber RX-TX-GND verbunden.
