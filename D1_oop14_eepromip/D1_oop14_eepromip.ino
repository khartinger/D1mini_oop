//_____D1_oop13_eepromip.ino________________20170601-170601_____
// Writing and reading data to internal EEPROM:
// write 2x ip address, then read 2x ip and show on Serial (9600)
// Hardware: WeMos D1 mini
#include "D1_class_Eepromip.h"
Eepromip myEEPROM(64);                 // eg. EEPROM size 64byte

void setup() {
 Serial.begin(9600);Serial.println("");// init serial com
 IPAddress ip1(192,168,1,1);
 IPAddress ip2(192,168,2,2);
 int anz;
 //-----write 2x ip---------------------------------------------
 anz=myEEPROM.writeIP(ip1, 32);
 if(anz!=4) Serial.println("Write ip1: ERROR");
 anz=myEEPROM.writeIP(ip2);
 if(anz!=4) Serial.println("Write ip2: ERROR");
 //-----read 2x ip----------------------------------------------
 IPAddress ip3, ip4;
 anz=myEEPROM.readIP(&ip3, 32);
 if(anz!=4) Serial.println("Read ip3: ERROR");
 anz=myEEPROM.readIP(&ip4);
 if(anz!=4) Serial.println("Read ip4: ERROR");
 //-----show ips------------------------------------------------
 Serial.print("ip3 = ");
 Serial.println(ip3);
 Serial.print("ip4 = ");
 Serial.println(ip4);
}

void loop() {
}
