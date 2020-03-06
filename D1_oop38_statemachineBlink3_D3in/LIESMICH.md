# D1 mini: Statemachine mit Blinklicht
Version 2020-03-06, Sketch: D1_oop38_statemachineBlink3_D3in.ino   
[English version](./README.md "English version")   

Dieses Programm zeigt die Ansteuerung einer LED mit Hilfe der Klassen `Statemachine` und `StatemachineBlink`. Der Modus wird mit Hilfe eines Tasters gew&auml;hlt:   
* Kurzes Dr&uuml;cken des Tasters D3 schaltet die blaue LED ein.   
* Langes Dr&uuml;cken des Tasters D3 schaltet die blaue LED aus.   
* Doppelklick des Tasters D3 l&auml;sst die blaue LED schnell blinken.

### Hardware
1. WeMos D1 mini
2. Button shield (D3)

### Software
Das Programm verwendet folgende Klassen:
* `Statemachine`: Zeitlicher Ablauf (10 States mit je 0,1 sec Dauer)   
* `StatemachineBlink`: Einschalten, Ausschalten und Blinken der LED   
* `Din`: Digitaler Eingang. Information, ob der Taster lang oder kurz gedr&uuml;ckt oder doppelt gedr&uuml;ckt wurde   
