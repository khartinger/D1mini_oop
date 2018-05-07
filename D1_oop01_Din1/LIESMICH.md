# D1 mini - Demo: Tasten-Klick Methoden der Klasse Din (digitale Eingabe, Version 2)
Sketch: D1_oop01_Din1.ino, Version 2017-12-13   
[English Version](./README.md "English Version")   

Zeige die Verwendung der Klick-Methoden der Klasse Din:   
* Bei fallender Flanke: Nachricht &uuml;ber serielle Schnittstelle
* Bei steigende Flanke: Nachricht &uuml;ber serielle Schnittstelle
* Wenn Taster l&auml;nger als eine Sekunde gedr&uuml;ckt wird: Nachricht &uuml;ber serielle Schnittstelle
* Wenn Taster einmal kurz gedr&uuml;ckt wird: Nachricht &uuml;ber serielle Schnittstelle
* Wenn Taster zweimal kurz gedr&uuml;ckt wird: Nachricht &uuml;ber serielle Schnittstelle
* Solange der Taster gedr&uuml;ckt wird: Schicke P &uuml;ber die serielle Schnittstelle

Zykluszeit ist 0,1 Sekunden, Ergebnisse werden an die serielle Schnittstelle gesendet (115200 Baud) und &uuml;ber die blaue LED angezeigt.

## Hardware
* WeMos D1 mini
* Button shield (D3) oder Draht von D3 zu GND

![Image: D1mini with button shield](./images/D1_buttonD3neu.png "D1mini with button shield")

&nbsp;

**Beispiel f&uuml;r die serielle Ausgabe:**
```
Press button D3!
v  Falling Edge
^  Rising Edge
Click!
v  Falling Edge
LongClick!
^  Rising Edge
v  Falling Edge
^  Rising Edge
v  Falling Edge
^  Rising Edge
DoubleClick!
```


## Klassendiagramm Din
| Klasse Din                    |                                               |
| ----------------------------- | --------------------------------------------- |
| + __Din__()                       | Vorgabe Konstruktor, setzt Pin auf D3, ruft setup() auf |
| + __Din__(int num)                | Setzen des Input Pins, Aufruf von setup()               |
| ~ void setup(int num)         | Setzt die Eigenschaften: input mode, lastMlli_, ...     |
| ===== *setter, getter Methoden (kein Hardwarezugriff)* ===== |                          |
| + void setClickMilli(unsigned long ms_max) | Maximale Dauer eines Teils eines Klicks    |
| + int  getLast(void)          | Gibt letzten Eingabewert zur&uuml;ck ohne neuerliches Einlesen |
| + unsigned long getDuration() | Gibt die Dauer des aktuellen Pinzustands in msec zur&uuml;ck (seit letzter Flanke) |
| + unsigned long getLastDuration() | Gibt die Dauer des vorherigen Pinzustands in msec zur&uuml;ck (ohne Pinabfrage) |
| ===== *Arbeitsmethoden* ===== |                   |
| + int  __read__(void)             | Liest Eingabepin ein, pr&uuml;ft auf Flanke, (Teile) der Klicks, R&uuml;ckgabe val |
| + bool __isPressed__(void)        | Ruft read() auf, gibt true (= Taste gedr&uuml;ckt = 0V) oder false (3V3) zur&uuml;ck |
| + bool __isPressed__(unsigned long ms_min) | Ruft read() auf, gibt true zur&uuml;ck, wenn der Taster l&auml;nger als die gegebenen Millisekunden gedr&uuml;ckt wird |
| + bool __isFallingEdge__(void)    | Ruft read() auf, gibt true zur&uuml;ck, wenn Taste gedr&uuml;ckt wurde  |
| + bool __isRisingEdge__(void)     | Ruft read() auf, gibt true zur&uuml;ck, wenn Taste losgelassen wurde |
| + bool __isClicked__()            | Ruft read() auf, gibt true zur&uuml;ck, wenn Taste 1x gedr&uuml;ckt wurde (Dauer kleiner als clickMilli_) |
| + bool __isDoubleClicked__()      | Ruft read() auf, gibt true zur&uuml;ck, wenn Taste 2x gedr&uuml;ckt wurde (Dauer kleiner als clickMilli_) |

&nbsp;

|  *Eigenschaften*              |                                               |
| ----------------------------- | --------------------------------------------- |
| ~ int dinNum_                 | Pinnummer (Vorgabe D3 = GPIO0)                |
| ~ int din_                    | Letzter eingelesener Wert des Pins (0 oder 1) |
| ~ int lastEdge_               | 0=Keine Flanke (Zustand nach der Abfrage der Flanke), 1=steigende Flanke, 2=Fallende Flanke |
| ~ int clickMilli_             | max. Dauer eines Klick(teiles) |
| ~ int click1                  | Status von 1x Klick (0=Bereit, 1=Fallende Flanke, 2=steigende Flanke, 3=Ende/ok |
| ~ int click2                  | Status von 2x Klick (0=Bereit, 1=Fallende Flanke, 2=steigende Flanke, 3=Fallende Flanke, 4=steigende Flanke, 5=Ende/ok |
| ~ int clickL                  | Status langer Klick (0=Bereit, 1=Fallende Flanke) |
| ~ unsigned long lastMilli_    | (Absoluter) Zeitpunkt des letzten Flankenwechsels in ms. Wert der Funktion milli() |
| ~ unsigned long lastDuration_ | Zeitdauer dieses Pinzustands in ms (seit dem letzten Pegelwechsel am Pin = Flanke) | 
