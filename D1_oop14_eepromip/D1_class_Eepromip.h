//_____D1_class_Eepromip.h__________________20170601-170603_____
// D1 mini class for using the internal eeprom.
// This class extends class Eeprom with writing/reading 
//   ip address.
#ifndef D1_CLASS_EEPROMIP_H
#define D1_CLASS_EEPROMIP_H
#include "D1_class_Eeprom.h"
#include "Ethernet.h"

class Eepromip : public Eeprom {
 public:
  Eepromip();
  Eepromip(uint32_t size1);
  int writeIP(IPAddress ip1, uint32_t address=EEPROM_SIZE_MAX+1);
  int readIP(IPAddress *ip1, uint32_t address=EEPROM_SIZE_MAX+1);
};

//_____constructor (default eeprom size is 80)__________________
Eepromip::Eepromip() { }

//_____constructor with eeprom size_____________________________
Eepromip::Eepromip(uint32_t size1):Eeprom(size1) { }

//_____write a ip address to eeprom (4 bytes)___________________
int Eepromip::writeIP(IPAddress ip1, uint32_t address)
{
 uint8_t block1[]={ip1[0],ip1[1],ip1[2],ip1[3]};
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 if(address+4>=size_) return 0;
 //-----write block---------------------------------------------
 return writeBlock(block1, address, 4);
}

//_____read a ip address from eeprom (4 bytes)__________________
int Eepromip::readIP(IPAddress *ip1, uint32_t address)
{
 uint8_t block1[4];
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 if(address+4>=size_) return 0;
 //-----read block----------------------------------------------
 int anz=readBlock(block1, address, 4);
 if(anz!=4) return 0;
 *ip1=IPAddress(block1[0],block1[1],block1[2],block1[3]);
 return 4;
}

#endif
