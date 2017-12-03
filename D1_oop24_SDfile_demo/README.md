# D1 mini: Demo - Write and read files on micro SD card
Sketch: D1_oop24_SDfile_demo.ino, Version 2017-12-03   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

The class SDfile is built on SD Library and helps to read and
write lines of a file on SD card.   
   
_Micro SD card agreements (for SD card library):_   
   FAT16 or FAT32, 8.3 format filename, NOT case-sensitive.

## Hardware
* WeMos D1 mini
* Data logger shield (microSD + DS1307RTC Shield)   
  DS1307  uses I2C (D1-SCL, D2-SDA)   
  microSD uses SPI (D5-CLK, D6-MISO, D7-MOSI, D8-CS)   


![Image: D1mini microSD + DS1307 shield](./images/D1_microSD_DS1307_shield.png "D1mini microSD + DS1307 shield")

**Example for Serial output:**
```
Initializing SD card.....Ready :)

09826c6d
-----micro SD card info---------------------
CardType: SDHC
Partition Fat32
VolumeSize: 14 GB
-----Demo file: /demo.txt-----
Demo file written!
Demo file :)
Does file '/demo.txt' exist: Yes
Remove file: success!
Does file '/demo.txt' exist: No

-----test writeLine, addLine, addFirstLine--
-----Show content of file /demo_new.txt-----
New first line :)
This is (old) line 1
And this is (old) line 2
Finally this is (old) line 3 :)
-----(End of file)--------------------------
First line |New first line :)|
Last line  |Finally this is (old) line 3 :)|
*****demo finished**************************
```


## Class diagram SDfile
| class SDfile |     |
| ---------------------------------  | -------- |
| + __SDfile__()                     | Constructor: set default filename, setup() |
| + __SDfile__(String filename)      | Constructor: filename, setup()             |
| ~ bool setup(String filename)      | init chipSelect (D8), check filename 8.3, init Serial, SD card, ... |
| ===== *setter and getter methods* ===== |     |
| + int    getStatus()                    | get status    |
| + bool   setFilename(String filename)   | set filename  |
| + String getFilename()                  | get filename  |
| ===== *file working methods* ===== |          |
| + bool   __exists__()              | check, if file exists   | 
| + bool   __remove__()              | remove file             |
| + bool   show()                    | show file content on Serial |
| + bool   __writeLine__(String line) | delete old file and write (first) line |
| + bool   __addLine__(String line)   | append a line          |
| + bool   addFirstLine(String line) | add a first line subsequently |
| + String __readFirstLine__()       | read first line of file |
| + String __readLastLine__()        | read last line of file  |
| + bool   writeDemofile()           | write a demo file, content "Demo file :)" |
| ===== *SD card info* =====         |      |
| + int    getCardType()             | card type: 1=SD1, 2=SD2, 3=SDHC, -1=Unknown, -2=init error |
| + int    getFatType()              | FAT type: 16 = FAT16, 32=FAT32, -1=unknown  |
| + int    getVolumeSize()           | volume size: 0=unknown, >0 GB, <0 MB |
| ===== *little helpers* =====       |      |
| + bool   is83(String filename)     | true: filename is 8.3 format |
| + bool   remove(String filename)   | remove given file |
| + String removeLFCR(String line)   | remove line feed and carriage return from line |
  
&nbsp;

|  *properties*               |                                         |
| --------------------------- | --------------------------------------- |
| - int  status               | 0=ok, >0 error                          |
| - String filename_          | filename (default: "/demo.txt")         |
| - int chipSelect            | default D8 (used by SD utility library) |
| ===== *just for SD card info* ===== |                                 |
| - Sd2Card  card             | for info about card                     |
| - SdVolume volume           | for info about volume                   |
| - bool     cardOK           | true: card object is available          |
| - bool     volumeOK         | true: volume object is available        |

  
  