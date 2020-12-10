//_____ESP32D1_01_miniWiFitest.ino____________201210-201210_____
// connect to WiFi and print WiFi.status to Serial
//#define D1MINI          1              // ESP8266 D1mini +pro
#define ESP32D1         2              // ESP32 D1mini

#if defined(ESP32) || defined(ESP32D1)
 #include "WiFi.h"                     // network connection
#else
 #include <ESP8266WiFi.h>              // network connection
#endif

char* ssid1 = "Raspi11";
const char* password1 = "12345678";
 
//_______SETUP__________________________________________________
void setup() {
  Serial.begin(115200); Serial.println("\nsetup(): --Start--");
  WiFi.begin(ssid1, password1);
  while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi network");
}

//_______LOOP___________________________________________________
void loop() {
 if(WiFi.status() == WL_CONNECTED)
  Serial.print("Connected to WiFi network ");
 else
  Serial.print("NOT connected to WiFi network "); 
 Serial.println(ssid1);
 delay(1000);
}
