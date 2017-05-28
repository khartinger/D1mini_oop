//_____D1_oop13_eeprom1.ino_________________20170521-170521_____
// Writing and reading data to internal EEPROM
// Hardware: WeMos D1 mini
#include "D1_class_Eeprom.h"
Eeprom myEEPROM(16);                   // eg. EEPROM size 16byte

void setup() {
 Serial.begin(9600);Serial.println("");// init serial com
 
 //-----test eeprom class: block write, read--------------------
 int anz;
 int blocksize=3;
 uint8_t block1[]={'A','B','C'};
 uint8_t block2[blocksize];
 anz=myEEPROM.writeBlock(block1, 0, blocksize);
 if(anz>0){Serial.print(anz); Serial.println(" bytes written.");}
 else Serial.println("Error writing block!");
 anz=myEEPROM.readBlock(block2, 0, blocksize);
 for(int i=0; i<anz; i++) Serial.print((char)block2[i]);
 Serial.print(" = ");
 for(int i=0; i<anz; i++) 
 { Serial.print(" 0x"); Serial.print(block2[i],HEX); }
 Serial.println(" => Block read.");
 
 //-----test eeprom class: int write, read----------------------
 int i1=-123, i2;
 anz=myEEPROM.writeInt(i1);
 if(anz>0) { Serial.print(i1); Serial.println(" written."); }
 else Serial.println("Error writing int!");
 myEEPROM.setAddress(myEEPROM.getAddress()-anz);
 anz=myEEPROM.readInt(&i2);
 if(anz>0) 
  { Serial.print(i2); Serial.println(" read."); }
 else 
 { Serial.print("Error reading int! i2="); Serial.println(i2); }
 
 //-----test eeprom class: int write, read----------------------
 i1=-11223;
 anz=myEEPROM.writeInt(i1,8);
 if(anz>0) { Serial.print(i1); Serial.println(" written."); }
 else Serial.println("Error writing int!");
 anz=myEEPROM.readInt(&i2,8);
 if(anz>0) 
  { Serial.print(i2); Serial.println(" read."); }
 else 
 { Serial.print("Error reading int! i2="); Serial.println(i2); }
 
 //-----test eeprom class: String write, read-------------------
 String s1="HALLO",s2;
 //myEEPROM.setAddress(8);
 anz=myEEPROM.writeString(s1, 8);
 if(anz>0) { Serial.print(s1); Serial.println(" written."); }
 else Serial.println("Error writing String!");
 //myEEPROM.setAddress(8);
 anz=myEEPROM.readString(&s2, anz, 8);
 if(anz>0)
  { Serial.print(s2); Serial.println(" read."); }
 else 
 { Serial.print("Error reading String! "); Serial.println(s2); }

 //-----test eeprom class: float write, read--------------------
 float f1=-3.1415926535, f2;
 //myEEPROM.setAddress(8);
 anz=myEEPROM.writeFloat(f1,8);
 if(anz>0) { Serial.print(f1,6); Serial.println(" written."); }
 else Serial.println("Error writing float!");
 //myEEPROM.setAddress(8);
 anz=myEEPROM.readFloat(&f2,8);
 if(anz>0) 
 { Serial.print(f2,6); Serial.println(" read."); }
 else 
 {Serial.print("Error reading float! f2="); Serial.println(f2);}

 char c1;
 myEEPROM.writeChar('X',0);
 myEEPROM.writeChar('Y');
 myEEPROM.writeChar('Z');
 Serial.print(myEEPROM.readChar(0));
 Serial.print(myEEPROM.readChar());
 Serial.print(myEEPROM.readChar());
 Serial.println();
}

void loop() {
}

