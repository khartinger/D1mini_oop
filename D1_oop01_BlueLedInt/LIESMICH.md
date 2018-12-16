# D1 mini: Demo f&uuml;r Blaue-LED-Klasse
Sketch: D1_oop01_BlueLedInt.ino, Version 2018-11-01   
[English Version](./README.md "English Version")   

Demo zu den Methoden der Klasse `BlueLedInt`:   
1. Blaue LED 2 sec einschalten, dann 1 sec ausschalten.   
2. Blaue LED langsam einschalten.   
3. Automatischess Blinken der LED w&auml;hrend einer Wartezeit (delay).   
4. Stoppen des Blinkens nach 2,5x (LED bleibt ein).   
5. Eine Sekunde warten und blinken wieder starten.   
6. Endlosschleife (loop): Automatisches Blinken mit 0,1s ein und 0,9s aus.

## Hardware
* WeMos D1 mini

![Bild: D1mini](./images/D1mini_171203.png "D1mini")

## Software

Die Pinnummer, der Pinzustand und die Zeiten f&uuml;r Blinken ein-aus werden in einer Struktur blueLedTimer gespeichert.
```
struct {
 int pinNum=BLUELEDINT_PIN;
 int pinVal=BLUELEDINT_OFF;
 int msOn  =BLUELEDINT_BLINK;
 int msOff =BLUELEDINT_BLINK;
} blueLedTimer;
```
<br>
| Klasse BlueLedInt                  |     |
| ---------------------------------- | --- |
| + BlueLedInt()            | Konstruktor mit Vorgabwerten (Digitaler Ausgangspin D4, nicht invertiert) |
| + BlueLedInt(int pin)     | Konstruktor mit angegebener Pinnummer (=GPIO), nicht invertiert |
| ~ void setup(int num) | Setup Output Pin und Eigenschaften |
| === *Setter-, Getter- und Arbeitsmethoden* === |     |
| + void set(int val)           | Setze den Ausgangspin physikalisch:  0=0V, 1=3V3 |
| + void on(void)               | Setze den Ausgangspin auf logisch ein (Ausgangswert 0|1 abh&auml;ngig von invert_) |
| + void off(void)              | Setze den Ausgangspin auf logisch aus (Ausgangswert 0|1 abh&auml;ngig von invert_) |
| + void toggle()               | Wechsle den Ausgangswert on <--> off bzw. 1 <--> 0  |
| + void blink(int on_ms,int off_ms) | Blinkdauer             |
| + void blinkOn()                   | Start blinken (nochmals) |
| + void blinkOff()                  | Stop blinken           |
| + void brightness(int persentOn)   | Helligkeit 0..100 [%]  |
| + void brightnessOn()              | Helligkeit ein         |
| + void brightnessOff()             | Helligkeit aus         |


  
&nbsp;

|  *Eigenschaften*            |                                         |
| --------------------------- | --------------------------------------- |
| ~ int  brightness_          | Helligkeit 0..100 [%]                   |
| ~ usePwm__                  | Wenn true: Helligkeitssteuerung ist ein |
