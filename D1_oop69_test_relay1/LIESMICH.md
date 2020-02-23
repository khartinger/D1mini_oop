# D1 mini: Test der Klasse Relay1
Version 2020-01-03 D1_oop69_test_relay1.ino    
[English version](./README.md "English version")   

Zum Testen der Klasse `Relay1` schaltet der D1 mini   
* ein Relais ein, um und aus und danach   
* eine Lampe ein, um und aus. (In diesem Fall ist das Schalten abh&auml;ngig vom gemessenen Strom)   

Das Ergebnis jeder Aktion und der gemessene Strom werden alle drei Sekunden &uuml;ber die serielle Schnittstelle ausgegeben:   
```   
2020-01-03 D1_oop69_test_relay1
Set relay (1) on: changed - relay is now 1
470.00 mA
Toggle relay (1): changed - relay is now 0
452.00 mA
Set relay (0) off: NOT changed - relay is now 0
452.00 mA
--------------------------------------------
Current max: 2000.00
Current on:  100.00
--------------------------------------------
Set lamp (1) on: NOT changed, i = 452.00mA, Lamp is now ON
Toggle lamp (1): changed, I = 468.00mA, Lamp is now ON
Set lamp (1) off: changed - 452.00mA, Lamp is now ON
452.00 mA
452.00 mA
452.00 mA
452.00 mA
452.00 mA
-------------------E N D--------------------
```   

### Hardware
1. WeMos D1 mini   
2. Relais Shield   
3. Stromwandler (zB ASM-010), Selbstbau-Shield D1_INA122_V3_191108 (oder D1_INA122_V2_180924) und eine Lampe zum Strommessen   
oder - nur zum Testen - ein Potenziometer/Trimmer am Analog-In-Eingang des D1 minis zum Simulieren des Stromes   

![D1 INA122 relay](./images/D1_ina122_relay.png "D1mini mit Selbstbau Shield INA122 und Relay Shield")   
_Bild 1: D1mini mit Eigenbau INA122 Shield, Relais Shield, Stromwandler ASM-010 und Lampe_   

![D1_test_relay1](./images/D1_test_relay1.png "D1mini mit Potenziometer und Relais Shield")   
_Bild 2: D1mini mit Potenziometer und Relais Shield_   

### Software
Die Klasse `Relay1` kann man entweder   
* das Relais direkt schalten (ein, aus oder um) oder
* den Lampenzustand schalten (ein, aus oder um).   
Der Unterschied ist, dass in diesem Fall das Relais so geschaltet wird, dass tats&auml;chlich ein Strom flie&szlig;t (oder nicht). Allerdings kann es bei fehlender Strommessung zu einem Fehler kommen:   
Bei fehlender Strommessung wird am Analogeingang der "Stromwert" 0 eingelesen. Leuchtet die Lampe und kommt der Befehl "Lampe aus", so wird das Relais NICHT umgeschaltet, da der Strom ja bereits "aus" ist.   

Anmerkung: Die Klasse `Relay1` verwendet zus&auml;tzlich die Klasse `Ain` zum Einlesen des Analogwertes.   

### Klassendiagramm `Relay1`

| Klasse Relay1                   |           |
| ------------------------------- | --------- |
| + Relay1() | Konstruktor (Vorgabe f&uuml; Relais-Pin is D1) |   
| + Relay1(int pinRelais) |Konstruktor, der die Pinnummer f&uuml;r das Relais setzt |   
| + Relay1(int pinRelais, float imax_mA, float ion_mA) | Konstruktor (setzt Relais-Pin, Maximalstrom und Strom, ab dem die Lampe als "ein" gilt) |   
| ~ void setup(int pin, float imax_mA, float ion_mA) | Setzt Relaispin, Maximalstrom und Strom, ab dem die Lampe als "ein" gilt. Schaltet das Relais aus |   
| ~ void measuring()                  | Messen des Stromes und ermitteln des Lampenzustandes |   
| __*Setter-/Gettermethoden*__ | <hr> |   
| bool   set(int val)                 | Versucht, Lampe ein- oder auszuschalten |   
| bool   setPinRelay(int pin)         | Setzt (neue) Relais-Pinnummer |
| void   setParams(int pinRelais, float imax_mA, float ion_mA) | Setzt Relais-Pin, Maximalstrom und Strom, ab dem die Lampe als "ein" gilt |   
| void   setCurrentOn(float mA)       | Setzt Mindeststrom f&uuml;r Lampenzustand "ein" |   
| void   setCurrentMax(float mA)      | Setzt Maximalstrom = 1023 AIN |   
| + bool   on(void)                     | Schaltet Relais ein (falls es nicht schon eingeschaltet ist) |   
| + bool   off(void)                    | Schaltet Relais aus (falls es nicht schon ausgeschaltet ist)|   
| + bool   toggle()                     | Schaltet Relais um |   
| + bool   lampOn(void)                 | Schaltet Lampe ein (falls sie laut Stromwert nicht schon eingeschaltet ist) |   
| + bool   lampOff(void)                | Schaltet Lampe aus (falls sie laut Stromwert nicht schon ausgeschaltet ist)|   
| + bool   lampToggle()                 | Schaltet Lampe um (falls sie laut Stromwert umgeschaltet werden muss)|   
| + bool   changed()                    | Abfrage, ob sich der Relais-/Lampen-Zustand seit der letzten Abfrage ge&auml;ndert hat (true=ja) |   
| + int    getRelaystate()              | Relais-Zustand (0=aus, 1=ein) |   
| + int    getLampstate()               | Lampen-Zustand (0=aus, 1=ein) |   
| + float  getCurrent()                 | Lampenstrom [mA] |   
| + float  getCurrentOn()               | Stromgrenzwert, ab dem der Strom als "ein" gilt |   
| + float  getCurrentMax()              | Maximaler Strom (f&uuml;r A/D-Wandler) |   
| + String getsStatusOled()             | Statustext mit maximal 10 Zeichen (zB zur Anzeige auf einem OLED 0.66") |   
| + unsigned long getDuration()         | Anzahl Millisekunden seit der letzten Zustands&auml;nderung |   

&nbsp;

|  *Eigenschaften*                |                                      |   
| ------------------------------- | ------------------------------------ |   
| ~ int   pinRelay_              | Relais Pin Nummer (default D1)        |   
| ~ int   relay_                 | Relais-Zustand (= Ausgabe von 0=0V, 1=3V3 am Relaispin) |   
| ~ int   lampstate_             | Lampenzustand laut Strommessung (1=ein, 0=aus) |   
| ~ bool  changed_               | &Auml;nderung des Relais-/Lampenzustandes seit der letzten Abfrage (beim Start false)  |   
| ~ float mA_                    | Aktueller Stromwert in mA |   
| ~ float mA_max_                | Maximaler Stromwert in mA (entspricht 1000 digit) |   
| ~ float mA_lamp_on_            | Stromgrenzwert, ab dem der Strom als "ein" gilt |   
| ~ Ain   ain                    | Objekt f&uuml;r die A/D-Wandlung |   
| ~ unsigned long lastMilli_     | Anzahl Millisekunden seit der letzten Zustands&auml;nderung |   
 

