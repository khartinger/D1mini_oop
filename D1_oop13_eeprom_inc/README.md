# D1_oop13_eeprom_inc 
This example shows how to write and read bytes to the internal D1 mini EEPROM using the class Eeprom, 
+ writing and reading with or without specification of start address
+ automatic increment of eeprom-pointer

The program creates a object myEEPROM with a size of 128 bytes (min. 4 bytes, max 4096 bytes), then
+ writes 3 chars, reads them back and shows them on Serial (9600 Baud)
+ writes 3 Strings, reads them back and shows them on Serial
+ writes 3 int numbers, reads them back and shows them on Serial
+ writes 3 float numbers, reads them back and shows them on Serial
+ writes 3 double numbers, reads them back and shows them on Serial

This program does not check, if reading or writing was successful.

## Hardware
* WeMos D1 mini

## Class diagram
| class Eeprom |
| ------------ |
| - uint32_t size_ <br> - uint32_t addr_ |
| - void setup(uint32_t size1) |
| + Eeprom() <br> + Eeprom(uint32_t size1) <br> + void setAddress(uint32_t address) <br> + uint32_t getAddress() <br> + size_t writeBlock(uint8_t *data, uint32_t address, size_t len) <br> + size_t readBlock(uint8_t *data, uint32_t address, size_t len) <br> + int writeChar(char c1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + char readChar(uint32_t address=EEPROM_SIZE_MAX+1) <br> + int writeString(String s1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readString(String *s1, int len, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int writeInt(int int1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readInt(int *int1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int writeFloat(float f1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readFloat(float *f1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int writeDouble(double f1, uint32_t address=EEPROM_SIZE_MAX+1) <br> + int readDouble(double *f1, uint32_t address=EEPROM_SIZE_MAX+1) |

Return value: number of bytes written or read (except constructors, setup, setAddress, getAddress, readChar).
