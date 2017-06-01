# D1_oop14_eepromip 
This example shows how to write and read ip addresses to the internal D1 mini EEPROM using the class Eepromip, especially
+ writing and reading with or without specification of start address
+ how to check, if reading or writing was successful.

The program creates a object myEEPROM with a size of 64 bytes (min. 4 bytes, max 4096 bytes), then
+ writes and reads 2 ips and shows them on Serial

## Hardware
* WeMos D1 mini

## Class diagram
| class Eepromip |
| -------------- |
| + Eepromip() <br> + Eepromip(uint32_t size1) |
| + int writeIP(IPAddress ip1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readIP(IPAddress *ip1, uint32_t address=EEPROM_SIZE_MAX+1) |

| class Eeprom |
| ------------ |
| ~ uint32_t size_ <br> ~ uint32_t addr_ |
| ~ void setup(uint32_t size1) |
| + Eeprom() <br> + Eeprom(uint32_t size1) <br> + void setAddress(uint32_t address) <br> + uint32_t getAddress() <br> + size_t writeBlock(uint8_t *data, uint32_t address, size_t len) <br> + size_t readBlock(uint8_t *data, uint32_t address, size_t len) <br> + int writeChar(char c1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + char readChar(uint32_t address=EEPROM_SIZE_MAX+1) <br> + int writeString(String s1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readString(String *s1, int len, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int writeInt(int int1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readInt(int *int1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int writeFloat(float f1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readFloat(float *f1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int writeDouble(double f1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readDouble(double *f1, uint32_t address=EEPROM_SIZE_MAX+1) |

Return value: number of bytes written or read (except constructors, setup, setAddress, getAddress, readChar).
