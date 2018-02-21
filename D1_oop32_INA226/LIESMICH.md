# D1 mini: Strom, Spannung und Leistung messen mit dem INA226
Sketch: D1_oop32_INA226.ino, Version 2018-02-18   
[English Version](./README.md "English Version")   

Das Beispielprogramm liest Spannung, Strom und Leistung &uuml;ber den I2C-Bus vom vom INA226 ein und zeigt die Werte &uuml;ber die serielle Schnittstelle an.

## Hardware
* WeMos D1 mini
* CJMCU-226 Board mit dem I2C-Bus des D1 mini verbinden: SCL=D1, SDA=D2, 3V3, GND

![D1mini und CJMCU-226 board](./images/D1_INA226.png "D1mini und CJMCU-226 board")   


##INA226
Der INA226 ist ein Strom- und Leistungsmesser mit I2C Schnittstelle.
Der IC kann sowohl den Spannungsabfall an einem Shunt-Widerstand als auch die Spannung am Verbraucher messen. Dabei darf der Shunt-Widerstand sowohl an der Versorgungsspannung als auch gegen Masse angebracht werden.
Ausserdem sind Kalibrierwerte, Umwandlungszeiten und die Mittelwertbildung einstellbar, sodass die Strom- und Leistungswerte (in Watt) direkt ausgelesen werden k&ouml;nnen.

| ___INA226___        | Wertebereich           | Toleranz           |
|:------------------- |:--------------------- :|:------------------ |   
| Versorgungsspannung | 2,7V bis 5,5V          | max. 6V            |
| Bus-Spannung        | 0V bis 36V             | max. -40V oder 40V |
| Shunt-Spannung      | -81,9175mV bis 81,92mV | -                  |

Software: Siehe auch https://github.com/jarzebski/Arduino-INA226
