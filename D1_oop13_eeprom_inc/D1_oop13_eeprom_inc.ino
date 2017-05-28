//_____D1_oop13_eeprom_inc.ino_______________20170522-170522_____
// Writing and reading data to internal EEPROM
// Hardware: WeMos D1 mini
#include "D1_class_Eeprom.h"
Eeprom myEEPROM(128);                  // eg. EEPROM size 128byte

void setup() {
 Serial.begin(9600);Serial.println("");// init serial com
 //-----test eeprom class: 3x write, 3x read char---------------
 char c1;
 myEEPROM.writeChar('X',0);
 myEEPROM.writeChar('Y');
 myEEPROM.writeChar('Z');
 Serial.print(myEEPROM.readChar(0));
 Serial.print(myEEPROM.readChar());
 Serial.print(myEEPROM.readChar());
 Serial.println();
 //-----test eeprom class: 3x write, 3x read String-------------
 String s1="Welt!", s2, s3;
 myEEPROM.writeString("Hallo",3);
 myEEPROM.writeString(" ");
 myEEPROM.writeString(s1);
 myEEPROM.readString(&s1,5,3);
 myEEPROM.readString(&s2,1);
 myEEPROM.readString(&s3,5);
 Serial.println(s1+s2+s3);
 //-----test eeprom class: 3x write, 3x read int----------------
 int i1=123, i2=-12345, i3;
 myEEPROM.writeInt(i1,16);
 myEEPROM.writeInt(i2);
 myEEPROM.writeInt(33333);
 myEEPROM.readInt(&i1,16);
 myEEPROM.readInt(&i2);
 myEEPROM.readInt(&i3);
 Serial.println("-----int--------");
 Serial.println(i1);
 Serial.println(i2);
 Serial.println(i3);
 //-----test eeprom class: 3x write, 3x read float--------------
 float f1=3.1415926535, f2=-11.2233, f3=1234567.89, f4, f5, f6;
 myEEPROM.writeFloat(f1,22);
 myEEPROM.writeFloat(f2);
 myEEPROM.writeFloat(f3);
 myEEPROM.readFloat(&f4,22);
 myEEPROM.readFloat(&f5);
 myEEPROM.readFloat(&f6);
 Serial.println("-----float------");
 Serial.println(f4,10);
 Serial.println(f5,4);
 Serial.println(f6);
 //-----test eeprom class: 3x write, 3x read double-------------
 double d1=3.1415926535, d2=-11.2233, d3=1234567.89, d4, d5, d6;
 myEEPROM.writeDouble(d1,40);
 myEEPROM.writeDouble(d2);
 myEEPROM.writeDouble(d3);
 myEEPROM.readDouble(&d4,40);
 myEEPROM.readDouble(&d5);
 myEEPROM.readDouble(&d6);
 Serial.println("-----double-----");
 Serial.println(d4,10);
 Serial.println(d5,4);
 Serial.println(d6);
}

void loop() {
}

