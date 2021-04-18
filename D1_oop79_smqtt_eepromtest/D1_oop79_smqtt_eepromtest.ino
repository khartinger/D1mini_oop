//_____D1_oop79_smqtt_eepromtest.ino__________201218-201218_____
// This program for a D1 mini (or ESP32 D1mini) tests writing 
// and reading from eeprom using class SimpleMqtt.
// Results are printed to Serial (115200Bd).

#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define TOPIC_BASE "led/1"
#define TOPIC_GET  "?,help,version,ip"
#define TOPIC_SET  ""
#define TOPIC_SUB  ""
#define TOPIC_PUB  ""

//_______MQTT communication_____________________________________
SimpleMqtt client("..ssid..", "..password..","mqttservername");

void callback(char* topic, byte* payload, unsigned int length)
{ client.callback_(topic, payload, length); }

String simpleGet(String sPayload) { return String(""); }
String simpleSet(String sTopic, String sPayload) { return String(""); }
void   simpleSub(String sTopic, String sPayload) { }

//_______SETUP__________________________________________________
void setup() {
 String s1w="topic";
 String s2w="C0=0.456";
 String s3w="TOP";
 String s1r, s2r, s3r;
 int iRet;
 //----- Serial-------------------------------------------------
 Serial.begin(115200); Serial.println("\nsetup(): --Start--");
 Serial.println("-----eepromEraseTopicBase-------------------");
 client.eepromEraseTopicBase();
 Serial.println("-----eepromEraseMyData----------------------");
 client.eepromEraseMyData();
 Serial.println("-----eepromWriteMyData----------------------");
 iRet=client.eepromWriteMyData(s2w);
 Serial.print("eepromWriteMyData(\""+s2w+"\"): ");
 Serial.println(client.getsEepromStatus(iRet));
 Serial.println("-----eepromWriteTopicBase-------------------");
 iRet=client.eepromWriteTopicBase(s1w);
 Serial.print("eepromWriteTopicBase(\""+s1w+"\"): ");
 Serial.println(client.getsEepromStatus(iRet));
 Serial.println("-----eepromReadTopicBase--------------------");
 s1r=client.eepromReadTopicBase(iRet);
 Serial.print("eepromReadTopicBase(): "+s1r+" --> ");
 if(s1w==s1r) Serial.print("equal --> ");
 else Serial.println("NOT equal --> ");
 Serial.println(client.getsEepromStatus(iRet));
 Serial.println("-----eepromReadMyData-----------------------");
 s2r=client.eepromReadMyData(iRet);
 Serial.print("eepromReadMyData(): "+s2r+" --> ");
 if(s2w==s2r) Serial.print("equal --> ");
 else Serial.println("NOT equal --> ");
 Serial.println(client.getsEepromStatus(iRet));
 Serial.println("-----write new, shorter topic base----------");
 Serial.println("-----eepromWriteTopicBase-------------------");
 iRet=client.eepromWriteTopicBase(s3w);
 Serial.print("eepromWriteTopicBase(\""+s3w+"\"): ");
 Serial.println(client.getsEepromStatus(iRet));
 Serial.println("-----eepromReadMyData-----------------------");
 s2r=client.eepromReadMyData(iRet);
 Serial.print("eepromReadMyData(): "+s2r+" --> ");
 if(s2w==s2r) Serial.print("equal --> ");
 else Serial.println("NOT equal --> ");
 Serial.println(client.getsEepromStatus(iRet));
 Serial.println("-----eepromEraseMyData----------------------");
 client.eepromEraseMyData();
 Serial.println("-----eepromEraseTopicBase-------------------");
 client.eepromEraseTopicBase();
 Serial.println("--------------------------------------------");
 //------setup finish-------------------------------------------
 Serial.println("setup(): --Finished--\n");
}

void loop() {
}