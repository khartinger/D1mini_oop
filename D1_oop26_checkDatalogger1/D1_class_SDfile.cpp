//_____D1_class_SDfile.cpp____________________171203-171203_____
// The class SDfile is built on SD Library and helps to read and
// write lines of a file on SD card.
// Micro SD card agreements (for SD card library):
//   FAT16 or FAT32, 8.3 format filename, NOT case-sensitive.
//
// Hardware: (1) WeMos D1 mini
//           (2) Data Logger Shield or Micro SD shield
//               Pins used by SPI bus:
//               D5=CLK, D6=MISO, D7=MOSI, D8=CS, 3V3, GND
//
// Based on code by Limor Fried and Tom Igoe. See
//   https://github.com/esp8266/Arduino/blob/master/
//         libraries/SD/examples/CardInfo/CardInfo.ino
// Created by Karl Hartinger, November 29, 2017.
// Released into the public domain.

#include "D1_class_SDfile.h"

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor 1____________________________________________
SDfile::SDfile(void) { setup(DEFAULT_FILENAME); }

//_____constructor 2____________________________________________
SDfile::SDfile(String filename) { setup(filename); }

//_____setup properties_________________________________________
bool SDfile::setup(String filename) {
 //-----object properties---------------------------------------
 chipSelect=SD_CHIP_SELECT;            // pin chip select SPI
 status=SDFILE_OK;
 //-----check filename 8.3--------------------------------------
  if(filename[0]!='/') filename="/"+filename;
 //-----check filename 8.3--------------------------------------
 if(!is83(filename)) {status=SDFILE_ERR_FILENAME; return false;}
 filename_=filename;                   // accept filename
 //-----do only once--------------------------------------------
 if(!SDfile_init)
 { 
  //-----begin Serial, read any existing Serial data------------
  if(!Serial) Serial.begin(BAUDRATE);  // init serial comm
  while (!Serial) {;};                 // wait for Serial
  do{delay(10);} while(Serial.available() && Serial.read()>=0);
  Serial.println();
  //-----check for micro SD card--------------------------------
  if(DEBUG1) Serial.print("Initializing SD card.....");
  if(!SD.begin(chipSelect))
  {
   if(DEBUG1) Serial.println("Failed. Is a card inserted?");
   status=SDFILE_ERR_INIT_SD;
   initOK=false;
   return false;
  }
  else
  {
   //-----just for SD card info---------------------------------
   cardOK=card.init(SPI_HALF_SPEED, chipSelect);
   if(cardOK) volumeOK=volume.init(card);
   else volumeOK=false;
   if(DEBUG1) Serial.println("Ready :)");
   SDfile_init=true;                   // sd card ready
   initOK=true;
  }
 }
 return true; 
}

//**************************************************************
//     setter and getter methods
//**************************************************************

//_____return status____________________________________________
int SDfile::getStatus() { return status; }

//_____set (new) filename_______________________________________
bool SDfile::setFilename(String filename)
 { return setup(filename); }

//_____return path+filename_____________________________________
String SDfile::getFilename() { return filename_; }

//**************************************************************
//     file working methods
//**************************************************************

//_____Tests whether this file exists on the SD card____________
bool SDfile::exists() { 
 char fn[FILENAME_MAX];
 filename_.toCharArray(fn, FILENAME_MAX);
 if(SD.exists(fn)) { status=SDFILE_OK; return true; }
 status=SDFILE_ERR_FILE_OP;
 return false;
}

//_____Remove this file from the SD card________________________
bool SDfile::remove() {
 char fn[FILENAME_MAX];
 filename_.toCharArray(fn, FILENAME_MAX);
 if(SD.remove(fn)) { status=SDFILE_OK; return true; }
 status=SDFILE_ERR_FILE_OP;
 return false;
}

//_____Remove this file from the SD card________________________
// This method does not effect status!
bool SDfile::remove(String filename) {
 char fn[FILENAME_MAX];
 filename.toCharArray(fn, FILENAME_MAX);
 if(SD.remove(fn)) return true;
 return false;
}

//_____prints file content to Serial____________________________
// return: true: data printed, false: nothing to print
bool SDfile::show() { 
 char c1;
 File _file;
 if(!exists())return false;            // no file
 _file=SD.open(filename_, FILE_READ);  // try to open file
 if(!_file) return false;              // error opening file
 //-----read chars from file until end--------------------------
 while (_file.available()) {           // while chars available
  Serial.write(_file.read());          // read, print to Serial
 }
 _file.close();                        // close file
 return true;
}

//_____create file and write a line (delete old file!)__________
bool SDfile::writeLine(String line) {
 remove();                             // remove old file
 return addLine(line);                 // add first line;
}

//_____add a line to file_______________________________________
bool SDfile::addLine(String line) {
 File _file;
 line=removeLFCR(line);
 _file=SD.open(filename_, FILE_WRITE); // try to open file
 if(_file)
 {
  _file.println(line);                 // print a line
  _file.close();                       // close file
 }
 else {
  status=SDFILE_ERR_FILE_OPEN; return false; 
 }
 return true;
}

//_____add a first line to file_________________________________
bool SDfile::addFirstLine(String line) {
 File _file;
 char c1;
 long i;
 unsigned long len1, pos1, pos2;
 len1=line.length();                   // without end of line
 //-----get old file size---------------------------------------
 _file=SD.open(filename_, FILE_WRITE); // try to open file
 if(!_file) return false;              // error opening file
 pos1=_file.size();                    // old file length
 _file.close();                        // close file
 //-----get new file size---------------------------------------
 addLine(line);                        // to make new file size
 _file=SD.open(filename_, FILE_WRITE); // try to open file
 if(!_file) return false;              // error opening file
 pos2=_file.size();                    // new file length
 for(i=pos1+len1; i<pos2; i++) {
  _file.seek(i);
  c1=_file.peek();
  line=line+c1;
 }
 _file.close();                        // close file
 len1=line.length();                   // length of line +EOL
 //-----build new file content----------------------------------
 _file=SD.open(filename_, FILE_WRITE); // try to open file
 if(!_file) return line;               // error opening file
 //-----move (old) file content to the end of file--------------
 for(i=pos1-1; i>=0; i--)
 {
  _file.seek(i);
  c1=_file.peek();
  _file.seek(i+len1);
  _file.print(c1);
 }
 //-----write new line to the begin of file----------------------
 _file.seek(0);
 for(i=0; i<len1; i++) _file.print(line[i]);

 _file.close();                       // close file
 return true;
}

//_____read first line of file__________________________________
String SDfile::readFirstLine() {
 File _file;
 bool carryon=true;
 char c1;
 String line="";
 if(!exists())return line;             // no file
 _file=SD.open(filename_, FILE_READ);  // try to open file
 if(!_file) return line;               // error opening file
 //-----read chars from file until end--------------------------
 while (_file.available()&& carryon)   // while chars available
 {
  c1=_file.read();
  if((c1=='\r') || (c1=='\n')) carryon=false;
  else line=line+c1;
 }
 _file.close();                        // close file
 return line; 
}

//_____read last line of file___________________________________
String SDfile::readLastLine() {
 File _file;
 bool carryon=true;
 char c1;
 String line="";
 int    _lfcr=0;
 unsigned long pos;
 if(!exists())return line;             // no file
 _file=SD.open(filename_, FILE_READ);  // try to open file
 if(!_file) return line;               // error opening file
 //-----read chars backwards from end of file until \r|\n-------
 pos=_file.size();
 if(pos==0) return line;               // empty file
 //-----remove last LF, CR--------------------------------------
 pos--;
 _file.seek(pos);
 c1=_file.peek();
 switch(c1) {
  case '\n': _lfcr=1; break;           // 1.char = \n
  case '\r': _lfcr=2; break;           // 1.char = \r
  default: line=line+c1;               // 1.char = other char
 }
 if(pos==0) return line;               // line = one char
 pos--;
 _file.seek(pos);
 c1=_file.peek();
 switch(c1) {
  case '\n': if(_lfcr!=2) return line; // 2x \n or 1char\n
             break;                    // \r\n
  case '\r': if(_lfcr!=1) return line; // 2x \r or 1char\r
             break;                    // \n\r
  default: line=line+c1;               // now 1.char
 }
 if(pos==0) return line;               // line = one char
 //-----now read chars until \r or \n---------------------------
 while((pos>0)&&carryon)
 {
  pos--;
  _file.seek(pos);
  c1=_file.peek();
  if((c1=='\r') || (c1=='\n')) carryon=false;
  else line=c1+line;
 }
 _file.close();                        // close file
 return line; 
}

//_____write a demo file________________________________________
bool SDfile::writeDemofile()
{
 File _file;
 remove();                             // remove old file
 _file=SD.open(filename_, FILE_WRITE); // try to open file
 if(!_file) {
 if(DEBUG1) Serial.println("Demo file: OPEN-Fehler!");   
  return false;                       // error opening file
 }
 _file.println("Demo file :)");
 //_file.write('\r');
 //_file.write('A');
 //_file.write('B');
 //_file.write('\n');
 _file.close();
 if(DEBUG1) Serial.println("Demo file written!"); 
 return true;
}

//**************************************************************
//     SD card info
//**************************************************************

//_____get type of SD card______________________________________
// return: 1=SD1, 2=SD2, 3=SDHC, -1=Unknown, -2=init error
int SDfile::getCardType()
{
 if (!cardOK)
 {
  if(DEBUG1) Serial.println("Card: initialization failed.");
  return -2;
 }
 switch (card.type()) {
  case SD_CARD_TYPE_SD1:
       if(DEBUG1) Serial.println("SD1");
       return 1;
  case SD_CARD_TYPE_SD2:
       if(DEBUG1) Serial.println("SD2");
       return 2;
  case SD_CARD_TYPE_SDHC:
      if(DEBUG1) Serial.println("SDHC");
      return 3;
  default:
      if(DEBUG1) Serial.println("Unknown");
 }
 return -1;
}

//_____get type FAT (16 or 32)__________________________________
// return: 16 = FAT16, 32=FAT32, -1=unknown
int SDfile::getFatType()
{
 if(!volumeOK) return -1;
 return volume.fatType();
}

//_____get volume size (in MB/GB)__________________________________
// return: 0=unknown, >0 GB, <0 MB
int SDfile::getVolumeSize()
{
 if(!volumeOK) return 0;
 unsigned long temp32;
 unsigned long volumesize;
 temp32=volume.blocksPerCluster();          // num of blocks
 volumesize=temp32;
 temp32=volume.clusterCount();              // num of clusters
 volumesize*=temp32;                        // * 512 = bytes
 volumesize/=2;                             // / 2   = KB
 volumesize/=1024;                          // MB
 if(volumesize<1024) return (-1)*((int)volumesize);
 volumesize /= 1024;
 return (int)volumesize;
}

//**************************************************************
//     little helpers
//**************************************************************

//_____is filename 8.3 format (return true=yes)_________________
bool SDfile::is83(String filename)
{
 String path, ext="";
 if(filename=="") return false;
 if(filename=="/") return false;
 int pos1=filename.lastIndexOf("/");
 int pos2=filename.lastIndexOf(".");
 path=filename.substring(0,pos1+1);
 if(pos2>0)
 {
  ext=filename.substring(pos2);
  if(ext.length()>4) return false;
  filename=filename.substring(pos1+1, pos2);
  if(filename.length()>8) return false;
 }
 else
 {
  filename=filename.substring(pos1+1);
  if(filename.length()>8) return false;
 }
 return true;
}

//_____remove line feed and carriage return from line___________
String SDfile::removeLFCR(String line)
{
 line.replace("\n", "");
 line.replace("\r", "");
 return line;
}

