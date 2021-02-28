# D1 mini: Anzeige von &uuml;ber die RS232 empfangenem Text auf OLED 0,66"
Sketch: D1_oop89_serial_screen13.ino, Version 2021-02-28   
[--> English Version](./README.md "English Version")   

Ein &uuml;ber die serielle Schnittstelle empfangener Text wird auf einer 0,66"-OLED-Anzeige angezeigt (Baudrate 115200Bd) und - &auml;hnlich der Darstellung auf dem OLED - &uuml;ber die serielle Schnittstelle wieder zur&uuml;ckgeschickt.   
Zur Anzeige wird `Screen13` aus der Klasse `Screen1` verwendet.  Dieser vorgefertigte Bildschirm hat eine doppelt gro&szlig;e erste Zeile mit 5 Zeichen und drei weitere Zeilen f&uuml;r je 10 Zeichen. Damit der Text in der richtigen Zeile angezeigt wird, m&uuml;ssen die einzelnen Zeilen durch ein Doppel-Unterstrich-Zeichen (`__`) getrennt werden. Ist ein Zeilentext l&auml;nger als 10 Zeichen, wird in der n&auml;chsten Zeile weitergeschrieben.   
Weiters blinkt die eingebaute blaue LED mit 1Hz, um anzuzeigen, dass der D1 mini l&auml;uft.   

__*Beispiele:*__   
`__` l&ouml;scht die OLED-Anzeige bzw. ergibt auf der seriellen Schnittstelle die Anzeige   
```
  |----------
1 |
```
-----   
`123__**Line 2**__  Line 3__    Line 4` ergibt die Anzeige   
```
  |----------
1 |123
2 |**Line 2**
3 |  Line 3
4 |    Line 4
```
-----   
`TITLE__Long text over several lines....__-x-x-x-` ergibt die Anzeige   
```
TITLE
Long text 
-x-x-x-
al lines..
```
das hei&szlig;t, der urspr&uuml;ngliche Inhalt der 3. Zeile ("over sever") wird &uuml;berschrieben. Als Ausgabe &uuml;ber die serielle Schnittstelle ergibt sich:   
```
  |----------
1 |TITLE
2 |Long text 
3 |over sever
4 |al lines..
3 |-x-x-x-
```   
 