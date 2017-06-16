//_____D1_class_Eeprom.h____________________20170521-170603_____
// D1 mini class for using the internal eeprom.
#ifndef D1_CLASS_EEPROM_H
#define D1_CLASS_EEPROM_H
#include <EEPROM.h>

#define EEPROM_SIZE                80       // arbitrary value
#define EEPROM_SIZE_MIN            4        // minimum size
#define EEPROM_SIZE_MAX            4096     // maximum size

class Eeprom {
 protected:
  uint32_t size_;
  uint32_t addr_;
 protected:
  void setup(uint32_t size1);
 public:
  Eeprom();
  Eeprom(uint32_t size1);
  void setAddress(uint32_t address);
  uint32_t getAddress();
  size_t writeBlock(uint8_t *data, uint32_t address, size_t len);
  size_t readBlock(uint8_t *data, uint32_t address, size_t len);
  int writeChar(char c1, uint32_t address=EEPROM_SIZE_MAX+1);
  char readChar(uint32_t address=EEPROM_SIZE_MAX+1);
  int writeString(String s1, uint32_t address=EEPROM_SIZE_MAX+1);
  int readString(String *s1, int len, uint32_t address=EEPROM_SIZE_MAX+1);
  int writeInt(int int1, uint32_t address=EEPROM_SIZE_MAX+1);
  int readInt(int *int1, uint32_t address=EEPROM_SIZE_MAX+1);
  int writeFloat(float f1, uint32_t address=EEPROM_SIZE_MAX+1);
  int readFloat(float *f1, uint32_t address=EEPROM_SIZE_MAX+1);
  int writeDouble(double f1, uint32_t address=EEPROM_SIZE_MAX+1);
  int readDouble(double *f1, uint32_t address=EEPROM_SIZE_MAX+1);
};

//_____constructor (default eeprom size is 80)__________________
Eeprom::Eeprom() { setup(EEPROM_SIZE); }

//_____constructor with eeprom size_____________________________
Eeprom::Eeprom(uint32_t size1) { setup(size1); }

//_____setup eeprom_____________________________________________
void Eeprom::setup(uint32_t size1)
{
 size_=size1;                               // used eeprom size
 if((size_<EEPROM_SIZE_MIN)||(size_>EEPROM_SIZE_MAX))
 size_=EEPROM_SIZE;
 addr_=0;                                   // start address
 EEPROM.begin(size_);
}

//_____setter method____________________________________________
void Eeprom::setAddress(uint32_t address) { addr_=address; }

//_____getter method____________________________________________
uint32_t Eeprom::getAddress() { return addr_; }

//_____write a block to eeprom__________________________________
// return: number of written bytes
size_t Eeprom::writeBlock(uint8_t *data, uint32_t address, size_t len)
{
 int i;
 //-----check input---------------------------------------------
 if(len<=0)    return 0;
 if(address<0) return 0;
 if(address>=size_) return 0;
 if((address+len)>size_) len=size_-address;
 //-----write bytes---------------------------------------------
 for(i=0; i<len; i++) 
  EEPROM.write(address+i, data[i]);
 EEPROM.commit();
 addr_=address+len;
 return len;
}

//_____read a block from eeprom_________________________________
// uses: EEPROM_SIZE
// return: number of read bytes or -1|-2|-3 on error
size_t Eeprom::readBlock(uint8_t *data, uint32_t address, size_t len)
{
 int i;
 //-----check input---------------------------------------------
 if(len<=0)    return 0;
 if(address<0) return 0;
 if(address>=size_) return 0;
 if((address+len)>size_) len=size_-address;
 //-----read bytes----------------------------------------------
 for(i=0; i<len; i++)
  data[i]=EEPROM.read(address+i);
 addr_=address+len;
 return len;
}

//_____write a char to eeprom___________________________________
//return: 0=not written, 1=byte written
int Eeprom::writeChar(char c1, uint32_t address)
{
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 if(address>=size_) return 0;
 //-----write char----------------------------------------------
 EEPROM.write(address, c1);
 EEPROM.commit();
 addr_=address+1;
 return 1;
}

//_____read a char from eeprom__________________________________
//return: char (or 0=error)
char Eeprom::readChar(uint32_t address)
{
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 if(address>=size_) return 0;
 //-----read char-----------------------------------------------
 char c1=(char)EEPROM.read(address);
 addr_=address+1;
 return c1;
}

//_____write String to eeprom at address________________________
// return: number of written bytes
int Eeprom::writeString(String s1, uint32_t address)
{
 int i,len_;
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 if(address>=size_) return 0;
 len_=s1.length();
 if((address+len_)>size_) len_=size_-address;
 //-----write bytes---------------------------------------------
 for(i=0; i<len_; i++)
  EEPROM.write(address+i, s1[i]);
 EEPROM.commit();
 addr_=address+len_;
 return len_;
}

//_____read String from eeprom at address_______________________
// return: number of read bytes
int Eeprom::readString(String *s1, int len, uint32_t address)
{
 int i;
 char c;
 String s0="";
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 if(address>=size_) return 0;
 if((address+len)>size_) len=size_-address;
 //-----read bytes----------------------------------------------
 for(i=0; i<len; i++) 
 {
  c=EEPROM.read(address+i);
  s0=s0+String(c);
 }
 *s1=s0;
 addr_=address+len;
 return len;
}

//_____write int number to eeprom at address____________________
// return: number of written bytes
int Eeprom::writeInt(int int1, uint32_t address)
{
 int i,len_;
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 if(address>=size_) return 0;
 len_=sizeof(int1);
 if((address+len_)>size_) return 0;
 //-----write int-----------------------------------------------
 uint8_t *bytebuffer = (uint8_t *)&int1;
 for(i=0; i<len_; i++)
  EEPROM.write(address+i, bytebuffer[i]);
 EEPROM.commit();
 addr_=address+len_;
 return len_; 
}

//_____read int number from eeprom at address___________________
// return: number of read bytes
int Eeprom::readInt(int *int1, uint32_t address)
{
 int i,len_;
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 len_=sizeof(int1);
 if(address+len_ > size_) return 0;
 //-----read int------------------------------------------------
 uint8_t *bytebuffer = (uint8_t *)int1;
 for(i=0; i<len_; i++)
  bytebuffer[i]=EEPROM.read(address+i);
 addr_=address+len_;
 return len_; 
}

//_____write float number to eeprom at address__________________
// return: number of written bytes
int Eeprom::writeFloat(float f1, uint32_t address)
{
 int i,len_;
 float f2;
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 len_=sizeof(f1);
 if(address+len_ > size_) return 0;
 //-----write float---------------------------------------------
 f2=EEPROM.put(address, f1);
 if(f1!=f2) len_=0;
 addr_=address+len_;
 return len_;
}

//_____read float number from eeprom at address_________________
// return: number of read bytes
int Eeprom::readFloat(float *f1, uint32_t address)
{
 int i,len_;
 float f2;
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 len_=sizeof(f1);
 if(address+len_ > size_) return 0;
 //-----read float----------------------------------------------
 f2=EEPROM.get(address, *f1);
 if(*f1!=f2) len_=0;
 addr_=address+len_;
 return len_;
}

//_____write double number to eeprom at address_________________
// return: number of written bytes
int Eeprom::writeDouble(double d1, uint32_t address)
{
 int i,len_;
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 if(address>=size_) return 0;
 len_=sizeof(d1);
 if((address+len_)>size_) return 0;
 //-----write double--------------------------------------------
 uint8_t *bytebuffer = (uint8_t *)&d1;
 for(i=0; i<len_; i++)
  EEPROM.write(address+i, bytebuffer[i]);
 EEPROM.commit();
 addr_=address+len_;
 return len_; 
}

//_____read double number from eeprom at address________________
// return: number of read bytes
int Eeprom::readDouble(double *d1, uint32_t address)
{
 int i,len_;
 double d2=0;
 //-----check input---------------------------------------------
 if(address==EEPROM_SIZE_MAX+1) address=addr_;
 if(address<0) return 0;
 len_=sizeof(d2);
 if(address+len_ > size_) return 0;
 //-----read double---------------------------------------------
 uint8_t *bytebuffer = (uint8_t *)d1;
 for(i=0; i<len_; i++)
  bytebuffer[i]=EEPROM.read(address+i);
 addr_=address+len_;
 return len_;
}
#endif
