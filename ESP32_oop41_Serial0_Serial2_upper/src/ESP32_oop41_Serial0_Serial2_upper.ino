//_____ESP32_oop41_Serial0_Serial2_upper.ino_____khartinger_____
// The program for the ESP32-mini waits for characters from 
// serial port #0 (Serial) and outputs them to Serial #2.
// Furthermore it waits for characters from Serial #2, converts 
// them to upper and sends them to Serial #0.
// Connect pin GPIO1 (TX) to GPIO16 (D4)
//     after programming ESP32-mini
// Hardware: (1) ESP32-WROOM-32 (= ESP32 D1 mini)
//           (2) 1x Wire
// Created by Karl Hartinger, November 23, 2021.
// Last Change:
// 2021-11-23 New
// See also https://quadmeup.com/arduino-esp32-and-3-hardware-serial-ports/
// Released into the public domain.

 #include <HardwareSerial.h>
 #define PIN_RXD        16             // 0:3, 2:16
 #define PIN_TXD        17             // 0:1, 2:17 (0...31)
HardwareSerial          mySeri(2);     // 0..2

#define   BUFFER_SIZE 80               // receive buffer size
uint8_t   charIn[BUFFER_SIZE];         // receive buffer

//_____setup____________________________________________________
void setup() 
{
 //-----start Serials-------------------------------------------
 Serial.begin(115200, SERIAL_8N1, 3, 1);
 mySeri.begin(115200,SERIAL_8N1,PIN_RXD,PIN_TXD);
}

//_____loop_____________________________________________________
void loop() {
 //-----Serial: check for received character(s) (from Terminal)-
 size_t len0 = Serial.available();    // byte(s) received?
 if(len0>0) {
  //-----Serial: read available bytes---------------------------
  if(len0 > BUFFER_SIZE) len0 = BUFFER_SIZE;
  Serial.readBytes(charIn, len0);
  //-----send chars to mySeri-----------------------------------
  mySeri.write(charIn, len0);
 }
 //-----mySeri: check for received character(s)-----------------
 size_t len2 = mySeri.available();     // byte(s) received?
 if(len2>0) {
  //-----mySeri: read available bytes---------------------------
  for(int i=0; i<len2; i++) {
   uint8_t c1 = mySeri.read();
   if((c1>96) && (c1<123)) c1=c1-32;    // to upper case
   //----send char to Serial (to Terminal)----------------------
   Serial.print(char(c1));
  }
 }
}
