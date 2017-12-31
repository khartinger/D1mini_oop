# D1 mini - Demo: Verschiedene Textbildschirme f&uuml;r OLED
Sketch: D1_oop02_oled_testall_V3.ino, Version 2017-12-30   
[English version](./README.md "English version")   

Dieses Beispielprogramm zeigt verschiedene Text-Bildschirm-Layouts unter Verwendung der Klasse __Screen1__ .   
Die Klasse __Screen1__ basiert auf den Klassen Adafruit_SSD1306 und Adafruit_GFX (und Klasse Print), wobei sich der gesamte, auf das OLED Shield abgespeckte Code in der Datei `D1_class_Screen_64x48.*` befindet.

## Hardware
* WeMos D1 mini
* OLED Shield: SSD1306, 64x48 pixel, I2C

## Bildschirm-Layouts
### Bildschirm-Namen

`screen##iClear`   
 ## ....... Anzahl der Textzeilen bei diesem Bildschirm (6, 5, 4, 2, 1 oder Kombinationen wie 221, 112, 13 12)   
 i .......... die Zeile wird inverviert dargestellt (schwarzer Text auf wei&szlig;em Hintergrund)   
 Clear ... L&ouml;schen des Bildschirms vor dem Schreiben der Zeile   

_Beispiele_
* screen4 ..... 4 Zeilen, normale Zeichendarstellung (wei&szlig; auf schwarz), Rand rundherum
* screen4i .... 4 Zeilen, invertierte Zeichendarstellung (schwarz auf wei&szlig;)
* screen13 .... 1 Zeile mit doppelt gro&szlig;en Zeichen, 3 Zeilen mit normaler Zeichendarstellung

Parameter
* line_ .... Zeilennummer, in die der Text geschrieben wird (1 bis 6 = Zeile 6)
* text_ .... Text, der geschrieben werden soll (max. 10 Zeichen pro Zeile, 5 Zeichen bei doppelt gro&szlig;en Zeichen)
* align_ ... Textausrichtung (Vorgabe: 'l'=linksb&uuml;ndig, Zeile wird vorher gel&ouml;scht. 'c'=zentriert, 'r'=rechtsb&uuml;ndig, 'L'=linksb&uuml;ndig+Zeichen &uuml;berschreiben, C'=zentriert+Zeichen &uuml;berschreiben, 'R'=rechtsb&uuml;ndig+Zeichen &uuml;berschreiben)

**Einfache Bildschirme (Screens): ein Bereich, 4 bis 6 Zeilen mit einfach hohen Zeichen**    
![D1mini screen6](./images/D1_oled_screen6.png "D1mini screen6")
![D1mini screen6i](./images/D1_oled_screen6i.png "D1mini screen6i")   
![D1mini screen5](./images/D1_oled_screen5.png "D1mini screen5")
![D1mini screen5i](./images/D1_oled_screen5i.png "D1mini screen5i")   
![D1mini screen4](./images/D1_oled_screen4.png "D1mini screen4")
![D1mini screen4i](./images/D1_oled_screen4i.png "D1mini screen4i")

**Einfache Bildschirme (Screens): ein Bereich, 1 bis 2 Zeilen mit doppelt hohen Zeichen**   
![D1mini screen2](./images/D1_oled_screen2.png "D1mini screen2")
![D1mini screen1a](./images/D1_oled_screen1a.png "D1mini screen1a")

**Bildschirme mit mehr als einem Bereich, einfach hohe Zeichen**      
![D1mini screen221](./images/D1_oled_screen221.png "D1mini screen221")

**Bildschirme mit einfach und doppelt hohen Zeichen**      
![D1mini screen112](./images/D1_oled_screen112.png "D1mini screen112")
![D1mini screen13](./images/D1_oled_screen13.png "D1mini screen13")
![D1mini screen12](./images/D1_oled_screen12.png "D1mini screen12")   

**Bildschirme mit zwei Symbolen f&uuml;r zwei Taster**      
![D1mini screen4B 1](./images/D1_oled_screen4B_1.png "D1mini screen4B 1")
![D1mini screen4B 2](./images/D1_oled_screen4B_2.png "D1mini screen4B 2")

**Spezielle Bildschirme**   
![D1mini dot_screen 1](./images/D1_oled_dot1.png "D1mini dot screen 1")
![D1mini dot_screen 2](./images/D1_oled_dot2.png "D1mini dot screen 2")
![D1mini dot_screen 3](./images/D1_oled_dot3.png "D1mini dot screen 3")


## Klassendiagramm Screen1

| Klasse Screen1               | erweitert _Adafruit_SSD1306, _Adafruit_GFX, Print    |
| ---------------------------- | ---------------------------------------------------- |
| + Screen1()                  | Vorgabekonstruktor, setzt OLED_RESET auf D3(=GPIO0), ruft setup() auf |
| + Screen1(int oled_resetpin) | Konstruktor, setzt OLED reset Pin, ruft setup() auf |
| ~ void setup(int pin_num)    | Eigenschaften setzen (I2C Adresse, Zeichensatz (font), Textfarbe, Textgr&ouml;&szlig;e, dotCounter)            |

&nbsp;

| Einfache Bildschirme: ein Bereich, 4 bis 6 Zeilen, einfach hohen Zeichen |                                                      |
| ------------------------------------------------------------------------ | ---------------------------------------------------- |
| + void screen6 (int line_, String text_)                    | Schreibe Text in Zeile 1...6, Text linksb&uuml;ndig, kein Rahmen  |
| + void screen6 (int line_, String text_, char align_)       | Schreibe Text in Zeile 1...6, angegebene Textausrichtung, kein Rahmen |
| + void screen6Clear (int line_, String text_)               | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...6, Text linksb&uuml;ndig, kein Rahmen  |
| + void screen6Clear (int line_, String text_, char align_)  | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...6, angegebene Textausrichtung, kein Rahmen |
| <hr> | <hr> |
| + void screen5 (int line_, String text_)                    | Schreibe Text in Zeile 1...5, Text linksb&uuml;ndig, Rahmen  |
| + void screen5 (int line_, String text_, char align_)       | Schreibe Text in Zeile 1...5, angegebene Textausrichtung, Rahmen |
| + void screen5Clear (int line_, String text_)               | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...5, Text linksb&uuml;ndig, Rahmen  |
| + void screen5Clear(int line_, String text_, char align_)   | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...5, angegebene Textausrichtung, Rahmen |
| <hr> | <hr> |
| + void screen4 (int line_, String text_)                    | Schreibe Text in Zeile 1...4, Text linksb&uuml;ndig, Rahmen  |
| + void screen4 (int line_, String text_, char align_)       | Schreibe Text in Zeile 1...4, angegebene Textausrichtung, Rahmen |
| + void screen4Clear (int line_, String text_)               | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...4, Text linksb&uuml;ndig, Rahmen  |
| + void screen4Clear (int line_, String text_, char align_)  | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...4, angegebene Textausrichtung, Rahmen |

__*Beachte*__: Bildschime mit i (wie screen6i, screen5i, screen4i) machen das gleiche, aber **invertieren** den Text in der angegebenen Zeile. Die Vorgabe f&uuml;r die Textausrichtung ist **zentriert**!

&nbsp;

| Einfache Bildschirme: ein Bereich, 1 bis 2 Zeilen, doppelt hohen Zeichen |       |
| ------------------------------------------------------------------------ | ----- |
| + void screen2 (int line_, String text_)                         | Schreibe Text in Zeile 1...2 (big, Rahmen), Text linksb&uuml;ndig  |
| + void screen2 (int line_, String text_, char align_)            | Schreibe Text in Zeile 1...2 (big, Rahmen), angegebene Textausrichtung |
| + void screen2Clear(int line_, String text_, char align_)        | Bildschirm l&ouml;schen, Schreibe Text in Zeile 1...2 (big, Rahmen), angegebene Textausrichtung |
| + void screen1 (String text_)                                    | Schreibe Text in Zeile 1 (big, Rahmen), Text mittenzentiert        |
| + void screen1 (String text_, char align_)                       | Schreibe Text in Zeile 1 (big, Rahmen), angegebene Textausrichtung |
| + void screen1Clear(String text_, char align_)                   | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...2 (big, Rahmen), angegebene Textausrichtung |


&nbsp;

| Bildschirme mit mehr als einem Bereich, einfach hohe Zeichen |       |
| ------------------------------------------------------------ | ----- |
| <nobr>+ void screen221(int line_,String text_, char align_, bool cls_)</nobr> | Schreibe Text in Zeile 1...5, angegebene Textausrichtung, Rahmen 1+2 und 3+4, Bildschirm l&ouml;schen |
| + void screen221(int line_,String text_)                         | Schreibe Text in Zeile 1...5, Text linksb&uuml;ndig,  Rahmen um Zeile 1+2 und 3+4 |
| + void screen221(int line_,String text_, char align_)            | Schreibe Text in Zeile 1...5, angegebene Textausrichtung, Rahmen um Zeile 1+2 und 3+4 |
| + void screen221Clear(int line_,String text_, char align_)       | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...5, angegebene Textausrichtung, Rahmen um Zeile 1+2 und 3+4 |

&nbsp;

| Bildschirme mit einfach und doppelt hohen Zeichen |                  |
| ------------------------------------------------- | ---------------- |
| <nobr>+ void screen112(int line_,String text_, char align_, bool cls_)</nobr> | Schreibe Text in Zeile 1...4 (Zeile 1 invertiert, line 2 big), Text linksb&uuml;ndig, Rahmen, Bildschirm l&ouml;schen |
| + void screen112(int line_,String text_)                         | Schreibe Text in Zeile 1...4 (Zeile 1 invertiert, line 2 big), Text linksb&uuml;ndig, Rahmen  |
| + void screen112(int line_,String text_, char align_)            | Schreibe Text in Zeile 1...4 (Zeile 1 invertiert, line 2 big), angegebene Textausrichtung, Rahmen  |
| + void screen112Clear(int line_,String text_, char align_)       | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...4 (Zeile 1 invertiert, line 2 big), angegebene Textausrichtung, Rahmen  |
| <hr> | <hr> | 
| + void screen13(int line_, String text_, char align_, bool cls_) | Schreibe Text in Zeile 1...4 (Zeile 1 doppelt hoch, Rahmen), angegebene Textausrichtung, Bildschirm l&ouml;schen |
| + void screen13(int line_, String text_)                         | Schreibe Text in Zeile 1...4 (Zeile 1 doppelt hoch, Rahmen), Text linksb&uuml;ndig  |
| + void screen13(int line_, String text_, char align_)            | Schreibe Text in Zeile 1...4 (Zeile 1 doppelt hoch, Rahmen), angegebene Textausrichtung |
| + void screen13Clear(int line_, String text_, char align_)       | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...4 (Zeile 1 doppelt hoch, Rahmen), angegebene Textausrichtung |
| <hr> | <hr> |
| + void screen12(int line_, String text_, char align_, bool cls_) | Schreibe Text in Zeile 1...3 (Zeile 1 doppelt hoch, Rahmen), angegebene Textausrichtung, Bildschirm l&ouml;schen |
| + void screen12(int line_, String text_)                         | Schreibe Text in Zeile 1...3 (Zeile 1 doppelt hoch, Rahmen), Text linksb&uuml;ndig  |
| + void screen12(int line_, String text_, char align_)            | Schreibe Text in Zeile 1...3 (Zeile 1 doppelt hoch, Rahmen), angegebene Textausrichtung |
| + void screen12Clear(int line_, String text_, char align_)       | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...3 (Zeile 1 doppelt hoch, Rahmen), angegebene Textausrichtung |

&nbsp;

| Bildschirme mit zwei Symbolen f&uuml;r zwei Taster |                  |
| -------------------------------------------------- | ---------------- |   
| + void screen4B (int line_, String text_)                    | Schreibe Text in Zeile 1...6, Text linksb&uuml;ndig, Rahmen  |
| + void screen4B (int line_, String text_, char align_)       | Schreibe Text in Zeile 1...6, angegebene Textausrichtung, Rahmen |
| + void screen4BClear (int line_, String text_)               | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...6, Text linksb&uuml;ndig, Rahmen  |
| <nobr>+ void screen4BClear (int line_, String text_, char align_)</nobr> | Bildschirm l&ouml;schen, schreibe Text in Zeile 1...6, angegebene Textausrichtung, Rahmen |

__*Beachte*__: Zeile 5 ist der linke Taster-Text, Zeile 6 ist der rechte Taster-Text. Der Taster-Text darf nur 4 Zeichen lang sein! Ist der Text leer, so wird das Tastersymbol nicht gezeichnet.

&nbsp;

| Spezial-Bildschirme            |                  |
| ------------------------------ | ---------------- |
| + void dotClear(void)          | Bildschirm l&ouml;schen, zeichne Rahmen                           |
| + void dotClear(String line6)  | Bildschirm l&ouml;schen, zeichne Rahmen, schreibe Text in Zeile 6 |
| + void dot(void)               | zeichne einen Punkt, erh&ouml;he dotCounter                       |
| + void dotLine(String line6)   | schreibe Text in Zeile 6                                          |

&nbsp;   

|   Hilfsfunktionen              |                                           |
| ------------------------------ | ----------------------------------------- |
| + String utf8ToOled(String s)  | Wandelt UTF-8 String In Oled ASCII Text um (Umwandlung spezieller deutscher Zeichen wie &auml;, &ouml;, &uuml;, &Auml;, &Ouml;, &Uuml;, &szlig;, ...) |
| ~ String mytrim(int max_, String txt_, char align)                         | Erstelle einen String mit max Zeichen (erg&auml;nze Leerzeichen davor und danach) |
| <nobr>~ void scr6(int line_, String text_, char align_, bool cls_, bool invert_)</nobr> | Display: 1 Bereich, 6 Zeilen, 10 Zeichen pro Zeile, kein Rahmen |
| ~ void scr5(int line_, String text_, char align_, bool cls_, bool invert_) | Display: 1 Bereich, 5 Zeilen, 10 Zeichen pro Zeile, kein Rahmen |
| ~ void scr4(int line_, String text_, char align_, bool cls_, bool invert_) | Display: 1 Bereich, 4 Zeilen, 10 Zeichen pro Zeile, kein Rahmen |
| ~ void screen2 (int line_, String text_, char align_, bool cls_) | Schreibe Text in Zeile 1...2 (big, Rahmen), angegebene Textausrichtung, Bildschirm l&ouml;schen |
| ~ void screen1 (String text_, char align_, bool cls_) | Schreibe Text in Zeile 1 (big, Rahmen), angegebene Textausrichtung, Bildschirm l&ouml;schen |

&nbsp;

|  *Eigenschaften*      |                                  |
| --------------------- | -------------------------------- |
| ~ int dotCounter      | counter for dot screen (0...50)  |
