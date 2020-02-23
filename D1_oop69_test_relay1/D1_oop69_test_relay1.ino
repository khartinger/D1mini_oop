//_____D1_oop69_test_relay1.ino_______________200103-200103_____
// Test for the class Relay1: A D1 mini switches a
// * relay on | toggle | off and afterwards a
// * lamp on | toggle | off. (In this case the switching is 
//   depending on the measured current)
// The result of every action and the measured current are
// written to serial interface (COM) every 3 seconds.
// @ Class Relay1:
// You can either change the relay state (on, off, toggle) or
// the lamp state (on, off, toggle). The difference is:
// Switching the "lamp" is based on a current measuring. 
// If no current is measured, the relay may not switch although 
// the lamp is on (because the current already seems to be off
// so the relay must not be switched!).
//
// Hardware: (1) WeMos D1 mini
//           (2) Relay shield
//           (3) Current transformer ASM-010, a selfmade INA122
//               shield and a lamp to measure the current
//               OR - just for test - a potentiometer/trimmer 
//               at Analog In pin to simulate the current
// Created by Karl Hartinger, January 03, 2020
// Modified: -
// Released into the public domain.

#include "src/relay1/D1_class_Relay1.h"
#define  VERSION69      "2020-01-03 D1_oop69_test_relay1"

//-----sensors, actors------------------------------------------
#define  PIN_RELAY     D1
Relay1  relay1(PIN_RELAY,2000.0,100.0); // relay shield

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 float mA;
 //-----start Serial--------------------------------------------
 Serial.begin(115200); //Serial.flush();      // empty out buf
 //while(Serial.available()) {Serial.read(); delay(2);}//empty in
 Serial.println("\n"+String(VERSION69));
 //-----set relay on--------------------------------------------
 Serial.print("Set relay ("); Serial.print(relay1.getRelaystate()); 
 Serial.print(") on: ");
 relay1.on();
 if(relay1.changed()) Serial.print("changed - ");
                 else Serial.print("NOT changed - ");
 Serial.print("relay is now "); Serial.println(relay1.getRelaystate());
 delay(2000);
 mA=relay1.getCurrent(); Serial.print(mA); Serial.println(" mA");
 delay(1000);

 //-----toggle relay--------------------------------------------
 Serial.print("Toggle relay ("); Serial.print(relay1.getRelaystate()); 
 Serial.print("): ");
 relay1.toggle();
 if(relay1.changed()) Serial.print("changed - ");
                 else Serial.print("NOT changed - ");
 Serial.print("relay is now "); Serial.println(relay1.getRelaystate());
 delay(2000);
 mA=relay1.getCurrent(); Serial.print(mA); Serial.println(" mA");
 delay(1000);
 //-----set relay off-------------------------------------------  
 Serial.print("Set relay ("); Serial.print(relay1.getRelaystate()); 
 Serial.print(") off: ");
 relay1.off();
 if(relay1.changed()) Serial.print("changed - ");
                 else Serial.print("NOT changed - ");
 Serial.print("relay is now "); Serial.println(relay1.getRelaystate());
 delay(2000);
 mA=relay1.getCurrent(); Serial.print(mA); Serial.println(" mA");
 delay(1000);

 //-----lamp methods--------------------------------------------
 Serial.println("--------------------------------------------");
 Serial.print("Current max: "); Serial.println(relay1.getCurrentMax());
 Serial.print("Current on:  "); Serial.println(relay1.getCurrentOn());
 Serial.println("--------------------------------------------");
 Serial.print("Set lamp ("); Serial.print(relay1.getLampstate()); 
 Serial.print(") on: ");
 relay1.lampOn();
 if(relay1.changed()) Serial.print("changed, i = ");
                 else Serial.print("NOT changed, i = ");
 mA=relay1.getCurrent(); Serial.print(mA);
 if(relay1.getLampstate()==1) Serial.println("mA, Lamp is now ON");
                         else Serial.println("mA, Lamp is now OFF");
 delay(3000);
 
 Serial.print("Toggle lamp ("); Serial.print(relay1.getLampstate()); 
 Serial.print("): ");
 relay1.lampToggle();
 if(relay1.changed()) Serial.print("changed, I = ");
                 else Serial.print("NOT changed, I = ");
 mA=relay1.getCurrent(); Serial.print(mA);
 if(relay1.getLampstate()==1) Serial.println("mA, Lamp is now ON");
                         else Serial.println("mA, Lamp is now OFF");
 delay(3000);

 Serial.print("Set lamp ("); Serial.print(relay1.getLampstate()); 
 Serial.print(") off: ");
 relay1.lampOff();
 if(relay1.changed()) Serial.print("changed - ");
                 else Serial.print("NOT changed - ");
 mA=relay1.getCurrent(); Serial.print(mA);
 if(relay1.getLampstate()==1) Serial.println("mA, Lamp is now ON");
                         else Serial.println("mA, Lamp is now OFF");
 delay(1000);
 mA=relay1.getCurrent(); Serial.print(mA); Serial.println(" mA");
 delay(1000);
 mA=relay1.getCurrent(); Serial.print(mA); Serial.println(" mA");
 delay(1000);
 mA=relay1.getCurrent(); Serial.print(mA); Serial.println(" mA");
 delay(1000);
 mA=relay1.getCurrent(); Serial.print(mA); Serial.println(" mA");
 delay(1000);
 mA=relay1.getCurrent(); Serial.print(mA); Serial.println(" mA");
 Serial.println("-------------------E N D--------------------");
}

//_____main loop________________________________________________
void loop() 
{
}
