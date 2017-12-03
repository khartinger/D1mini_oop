//_____D1_oop24_SDfile_demo.ino_______________171203-171203_____
// Demo for the class SDfile to show all file operations
// on a Micro SD shield for D1 mini.
// Micro SD card agreements (for SD card library):
//   FAT16 or FAT32, 8.3 format filenames, NOT case-sensitive.
//
// Hardware: (1) WeMos D1 mini
//           (2) Data Logger Shield or Micro SD shield
//               Pins used by SPI bus:
//               D5=CLK, D6=MISO, D7=MOSI, D8=CS, 3V3, GND
//
#include "D1_class_SDfile.h"
SDfile file1;

//_____setup routine____________________________________________
void setup(void){
 String s1="This \r\nis \n(old) line \n\r1\r\n";
 String line;
 Serial.println("-----micro SD card info---------------------");
 Serial.print("CardType: "); file1.getCardType();
 Serial.print("Partition Fat"); Serial.println(file1.getFatType());
 Serial.print("VolumeSize: "); 
 int vsize=file1.getVolumeSize();
 if(vsize>0) { Serial.print( vsize); Serial.println(" GB"); }
        else { Serial.print(-vsize); Serial.println(" MB"); }
 
 //-----write demofile and check, if file exists----------------
 Serial.println("-----Demo file: "+file1.getFilename()+"-----");
 file1.writeDemofile();
 file1.show();
 Serial.print("Does file '"+file1.getFilename()+"' exist: ");
 if(file1.exists()) Serial.println("Yes");
 else Serial.println("No");
 //-----remove file and check deletion--------------------------
 Serial.print("Remove file: ");
 if(file1.remove()) Serial.println("success!");
 else Serial.println("failed!");
 Serial.print("Does file '"+file1.getFilename()+"' exist: ");
 if(file1.exists()) Serial.println("Yes");
 else Serial.println("No");
 Serial.println(); 
 //-----write a file with 3/4 lines and show content on Serial--
 Serial.println("-----test writeLine, addLine, addFirstLine--");
 file1.setFilename("demo_new.txt");
 file1.writeLine(s1);
 file1.addLine("And this is (old) line 2");
 file1.addLine("Finally this is (old) line 3 :)");
 file1.addFirstLine("New first line :)");
 Serial.print("-----Show content of file ");
 Serial.println(file1.getFilename()+"-----");
 file1.show();
 Serial.println("-----(End of file)--------------------------");
 line=file1.readFirstLine();
 Serial.println("First line |"+line+"|");
 line=file1.readLastLine();
 Serial.println("Last line  |"+line+"|");
 Serial.println("*****demo finished**************************");
}

//_____main loop________________________________________________
void loop(void){
 //...nothing to do...

}

