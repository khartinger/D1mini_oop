# D1 mini: Statemachine mit Blinklicht
Version 2020-02-25, Datei: D1_oop38_statemachineBlink.ino   
[English version](./README.md "English version")   

Dieses Programm zeigt, wie man mit Hilfe der Klassen `Statemachine` und `StatemachineBlink` ein Blinklicht realisiert.   
Zum Erstellen des Blinkobjekts ben&ouml;tigt man folgende Informationen:   
* Die Nummer des Pins, an dem die LED angeschlossen ist (zB D4)   
* Leuchtet die LED, wenn der Pin auf 3V3 liegt oder bei 0V (= invertiert)?   
* Die State-Nummer, bei der die LED zum ersten Mal zu leuchten beginnen soll   
* die Dauer des LED-ein-Zustandes (in States)   
* die Dauer des LED-aus-Zustandes (in States)   
* die Anzahl der Blinker (-1 bedeutet Dauerblinken)

### Hardware
1. WeMos D1 mini

### Software
Das Programm verwendet folgende Klassen:
* `Statemachine`: Zeitlicher Ablauf (120 States mit je 0,1 sec Dauer)   
* `StatemachineBlink`: Ein- und Ausschalten der LED   

### Konstruktor f&uuml;r das Blinklicht
`StatemachineBlink led1(LED1_PIN, LED1_INVERT, STATE_LED1_START, STATES_LED1_ON, STATES_LED1_OFF, LED1_BLINK_NUM);`   
__*Beispiele:*__   
_Annahme_: Die Dauer eines States der Statemachine ist 0,1 sec   
* `StatemachineBlink led1(D4,true,1,5,5,3);` ... Die blaue LED blinkt bei jedem Statemachine-Zyklusstart 3x im Sekundentakt
* `StatemachineBlink led1(D4,true,20,1,9,5);` ... Die blaue LED blitzt zwei Sekunden nach Beginn des Statemachine-Zyklusstarts 5x kurz auf
* `StatemachineBlink led7(D7,false,1,5,15,-1);` und   
`StatemachineBlink led8(D8,false,6,15,5,-1);` ... Die LEDs an D7 und D8 bilden ein Endlos-Wechselblinklicht, wobei die LED D8 dreimal so lange eingeschaltet ist wie D7


### (Main-)loop
Die (Endlos-)loop()-Funktion eines Blinklichts lautet z.B.   
```
void loop() {
 statemachine.loopBegin();                  // state begin
 led1.doBlink(statemachine);                // blink LED
 statemachine.loopEnd();                    // state end
}
```
