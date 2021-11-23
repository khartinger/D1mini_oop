//_____ESP32_ex25_serial_bytes_io.ino____________khartinger_____
// Read byte(s) from Serial #0 or #1 or #2 (115200 Bd), 
// convert them to upper and send them back (echo).
// Blue LED toggles, if byte(s) is/are received.
// Test the program:
// 1. connect USB serial adapter to PC
// 2. connect USB-Serial-Adapter with ESP32-mini pins PIN_RXD 
//    and PIN_TXD (cross out, i.e. TXD <--> RXD)
// 3. PC: start terminal program, select COM and send data
// Hardware: (1) ESP32-mini (ESP32-WROOM-32)
//           (2) 3V3 USB-Serial-Adapter + lines
// Created by Karl Hartinger, November 23, 2021.
// Last Change:
// 2021-11-23 New
// Released into the public domain.

#include <HardwareSerial.h>
/*
#define _SERI_         Serial         // Serial1, Serial2
#define PIN_RXD        1              // 0:3, 2:16
#define PIN_TXD        3              // 0:1, 2:17 (0...31)
*/
#define _SERI_          mySerial       //
#define PIN_RXD         25             // 0:3, 2:16/25
#define PIN_TXD         27             // 0:1, 2:17/27 (0...31)
HardwareSerial          _SERI_(2);     // 0..2

#define BUFFER_SIZE     80             // receive buffer size
uint8_t buffer[BUFFER_SIZE];           // receive buffer
const int pinLED=2;                    // blue led on board

//_______setup__________________________________________________
void setup() 
{
 //------init pins----------------------------------------------
  pinMode(pinLED, OUTPUT);              // set led pin as output
 //------start _SERI_-------------------------------------------
 _SERI_.begin(115200,SERIAL_8N1,PIN_RXD,PIN_TXD); //start Serial
}

//_____loop_____________________________________________________
void loop() {
 //-----_SERI_: check for received character(s) (from Terminal)-
 size_t len_ = _SERI_.available();    // byte(s) received?
 if(len_>0) {
   digitalWrite(pinLED, digitalRead(pinLED)^1);    // invert LED
  //-----_SERI_: read available bytes---------------------------
  if(len_ > BUFFER_SIZE) len_ = BUFFER_SIZE;
  _SERI_.readBytes(buffer, len_);
  //-----convert to upper---------------------------------------
  for(int i=0; i<len_; i++) 
   buffer[i]=toupper(buffer[i]);
  //-----write bytes back---------------------------------------
  //for(int i=0; i<len_; i++) 
  _SERI_.write(buffer, len_);
 }
}
