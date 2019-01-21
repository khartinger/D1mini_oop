//_____D1_oop63_Wifimeter_oled.ino____________190119-190119_____
// Display WiFi strength on an OLED screen221.
// Hardware: (1) WeMos D1 mini
//           (2) 0.66"-OLED-Shield
// Created by Karl Hartinger, January 19, 2019.
// Last modified: -
// Released into the public domain.
#define  DEBUG63    true              //
#define  VERSION63  "2019-01-19 D1_oop63_Wifimeter_oled"
#define  FUNCTION63 "Display WiFi strength on oled."
#define  OLED_RESET            RST     // D3=GPIO0, D0=GPIO16

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "src/screen1a/D1_class_screen1a.h"

const char* ssid = "Raspi11";          // (17 chars)
const char* password = "12345678";     // min. 8 chars or ""
char* host = "10.1.1.1";               // or "http://..."
IPAddress ip(10,1,1,99);               //
IPAddress gateway(10,1,1,1);           //
IPAddress subnet(255,255,255,0);       //
int httpPort=80;                       // http port
WiFiClient client;                     // 
Screen1   display_;                    // OLED 0.66"
String    sSSID=String(ssid).substring(0,10);

//-----connect to WLAN?-----------------------------------------
// yes; return true, no try to connect (endless)
bool wifiConnected()
{
 if(client.connect(host, httpPort)) return true;
 display_.dotClear("connecting");
 if(DEBUG63) Serial.println("connecting...");
 //-----Wait for connection-------------------------------------
 int CRcounter=0;
 while (WiFi.status() != WL_CONNECTED) {
  if(DEBUG63) Serial.print(".");
  display_.dot();
  if((++CRcounter)>60) { CRcounter=0; Serial.println(); }
  delay(500);
 }
 display_.screen112(1,"Signal dBm",'c');
 display_.screen112(2,"?",'c');
 display_.screen112(3,sSSID,'c');
 display_.screen112(4,"connected",'c');
 return true;
}

//_____setup routine____________________________________________
void setup(void){
 if(DEBUG63) Serial.begin(115200);
 if(DEBUG63) Serial.println("\n-----Setup wifi connection-----");
 //-----Setup wifi connection-----------------------------------
 WiFi.mode(WIFI_STA);
 WiFi.config(ip, gateway, subnet);  // skipp for dhcp
 WiFi.begin(ssid, password);
}

//_____main loop________________________________________________
void loop(void){
 if(wifiConnected())
 {
  //-----display on OLED----------------------------------------
  String sRsii=String(WiFi.RSSI());
  display_.screen112(2,sRsii,'c');
  //-----send connection info to serial interface---------------
  if(DEBUG63)
  {
   Serial.print("\r\nConnected to "); Serial.println(ssid);
   Serial.println("Signal strength: "+sRsii+"dBm");
   Serial.print("Client IP address: ");
   Serial.println(WiFi.localIP());
  }
 }
 else
 {
  Serial.println("Not connected");
 }
 delay(100);
}
