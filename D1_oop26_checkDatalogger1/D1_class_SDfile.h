//_____D1_class_SDfile.h______________________171203-171203_____
// The class SDfile helps to write data to a file on SD card.
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

#ifndef D1_CLASS_SDFILE_H
#define D1_CLASS_SDFILE_H
#include "Arduino.h"              //
#include <SPI.h>                  // include SPI library
#include <SD.h>                   // include SD library

#ifndef DEBUG1
 #define DEBUG1               false
#endif
#define BAUDRATE             115200
#define SD_CHIP_SELECT           D8
#define DEFAULT_FILENAME  "/demo.txt"  // 8.3 +must start with /
#define FILENAME_MAX             64    // LINUX-Default: 4096 !!
#define EOL                    "\n"    // new line

//-------status-------------------------------------------------
#define  SDFILE_OK                0    // success
#define  SDFILE_ERR_INIT_SD       1
#define  SDFILE_ERR_MOUNT_FS      2    // error mount filesystem
#define  SDFILE_ERR_FILENAME      3    // 
#define  SDFILE_ERR_FILE_OPEN     4    // error file open
#define  SDFILE_ERR_FILE_OP       5    // error file operation
#define  SDFILE_ERR_NO_FILE       6    // file does not exist

static bool SDfile_init=false;

class SDfile {
 //-----properties----------------------------------------------
 protected: 
  int      status;
  String   filename_;
  bool     initOK;
 //-----properties, used by SD utility library functions--------
  int      chipSelect;
 //-----properties just for SD card info------------------------
  Sd2Card  card;
  SdVolume volume;
  bool     cardOK;
  bool     volumeOK;
 //-----constructor & co----------------------------------------
 public:
         SDfile(void);
         SDfile(String filename);
 protected:
  bool   setup(String filename);
 //-----setter and getter methods-------------------------------
 public:
  int    getStatus();
  bool   setFilename(String filename);
  String getFilename();
 //-----file working methods------------------------------------
  bool   exists(); 
  bool   remove();
  bool   show();
  bool   writeLine(String line);
  bool   addLine(String line);
  bool   addFirstLine(String line);
  String readFirstLine();
  String readLastLine();
  bool   writeDemofile();
  //-----SD card info-------------------------------------------
  int    getCardType();
  int    getFatType();
  int    getVolumeSize();
  //-----little helpers-----------------------------------------
  bool   is83(String filename);
  bool   remove(String filename);
  String removeLFCR(String line);
};
#endif
