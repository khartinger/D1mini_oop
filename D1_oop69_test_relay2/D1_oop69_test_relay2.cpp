//_____D1_oop69_test_relay2.ino_______________201210-201210_____
// Test for the class Relay2: A D1 mini or ESP32 Dimini
// switches a
// * relay on | toggle | off
// The result of every action is written to serial interface
// (COM) every 3 seconds (115200Bd)
//
// Hardware:
// (1) WeMos D1 mini
// (2) Relais shield (D1)
// Created by Karl Hartinger, December 10, 2020
// Updates:
// 2020-12-10 New
// Released into the public domain.

#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini

#include "src/relay2/D1_class_Relay2.h"
#define  VERSION69      "2020-12-10 D1_oop69_test_relay2"

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define  PIN_RELAY     22
#else
 #define  PIN_RELAY     D1
#endif
Relay2   relay2(PIN_RELAY);           // relais 1=on

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 //-----start Serial--------------------------------------------
 //if(DEBUG69)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //-------set actors--------------------------------------------
 Serial.print("Set relay on:  relay is now ");
 relay2.on();
 if(relay2.isOn()) Serial.print("ON  - switchStatus=");
              else Serial.print("OFF - switchStatus=");
 int switchStatus=relay2.getSwitchStatus();
 Serial.println(switchStatus);
 delay(1500);
 //-----toggle relay--------------------------------------------
 Serial.print("Toggle relay:  relay state has ");
 relay2.toggle();
 if(relay2.isChange()) Serial.print("changed: ");
                  else Serial.print("NOT changed: ");
 if(relay2.isOff())    Serial.print("OFF - switchStatus=");
                  else Serial.print("ON  - switchStatus=");
 switchStatus=relay2.getSwitchStatus();
 Serial.println(switchStatus);
 delay(1500);
 //-----toggle relay--------------------------------------------
 Serial.print("Toggle relay:  relay state has ");
 relay2.toggle();
 if(relay2.isChange()) Serial.print("changed: ");
                  else Serial.print("NOT changed: ");
 if(relay2.isOn())     Serial.print("ON  - switchStatus=");
                  else Serial.print("OFF - switchStatus=");
 switchStatus=relay2.getSwitchStatus();
 Serial.println(switchStatus);
 delay(1500);
 //-----set relay off-------------------------------------------  
 Serial.print("Set relay off: relay is now ");
 relay2.off();
 if(relay2.isOff()) Serial.print("OFF - switchStatus=");
               else Serial.print("ON  - switchStatus=");
 switchStatus=relay2.getSwitchStatus();
 Serial.println(switchStatus);
 delay(1500);
 Serial.println("-------------------E N D--------------------");
}

//_____main loop________________________________________________
void loop() 
{
}
