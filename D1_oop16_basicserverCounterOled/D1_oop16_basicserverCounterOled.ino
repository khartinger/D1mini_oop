//_____D1_oop16_basicserverCounterOled.ino____170605-170625_____
// Host listens to Port 80, increments counter on request
// and sends back counter value. 
// Button on D5 increments counter locally or resets counter
// D1mini blue LED on indicates no connection to WLAN (but 
//  trying to connect). When connected blue LED is off.
// HTTP-Answer:            counter value
// OLED-Display:           counter value and client ip
// Serial monitor:         log messages (9600Bd)
// Hardware: WeMos D1 Mini
//           OLED Shield: SSD1306, 64x48 pixel, I2C
// Uses Adafruit libs Adafruit_SSD1306-esp8266-64x48.zip and
//                    Adafruit-GFX-Library-1.1.5.zip

#include "D1_class_DisplayKH.h"
#include "D1_class_Din.h"
#define  TITLE_ "D1_oop16_basicserverCounterOled"
#include "D1_class_BasicserverKH.h"
#define  PIN_BUTTON               D5   // PIN_BUTTON=D5=GPIO14
#define  BUTTON_PRESSED_FOR_RESET 2000 // 2000ms
DisplayKH     display_;                // 
BasicserverKH server(80);              // 
Din           button(PIN_BUTTON);      // counter inc|reset
unsigned long requestCounter=0;
unsigned long endMillis;
int last_=-1;

//_____display_status___________________________________________
void display_status(int status)
{
 //-----counter value as string---------------------------------
 String sCounter=String(requestCounter);
 //-----text below----------------------------------------------
 String sIPserver=WiFi.localIP().toString();
 //String sIPclient= server.client().remoteIP().toString();
 String sIPclient= server.getLastClient().toString();
 String sRssi = String(WiFi.RSSI());
 String s2="";
 String s3="";
 switch(status)
 {
  case  0: break;                      // do nothing
  case  1: sCounter="ERROR";
           s2="no connect";
           s3=String(server.getSSID()); break;
  case  2: s2="This Host:";
           s3=sIPserver;
           break;
  case  3: s2="New client";
           s3=sIPclient+"|"+sRssi+"dBm";
           break;
  case  5: s2="--reset---";
           s3=sIPserver+"          ";
           break;
  case  6: s2="--local---";
           s3=sIPserver+"          ";
           break;
  case  7: sCounter="  ?  ";
           s2="searching ";
           s3=String(server.getSSID()); break;
  default: s2="Unknown   ";
           s3="state "+String(status)+"   ";
           break;
 }
 //-----display values------------------------------------------
 display_.screen13(1,sCounter,'c',false);
 if(s2!="") display_.screen13(2,s2,'l',false);
 if(s3!="") display_.screen13(3,s3,'l',false);
 display_.display();
 //-----show status on Serial-----------------------------------
 Serial.println("Status "+String(status)+": "+s2+", counter="+sCounter);
}

//_____increment counter by button______________________________
void doInterrupt(void)
{
 if(button.isPressed())
 { //-----falling edge------------------------------------------
  requestCounter++;
  display_status(6);
  endMillis = millis()+BUTTON_PRESSED_FOR_RESET;
  Serial.println("Falling Edge: "+String(requestCounter));
 }
 else
 { //-----rising edge-------------------------------------------
  if(millis() > endMillis)
  {
   requestCounter=0;
   display_status(5);
   Serial.println("Rising Edge: "+String(requestCounter));
  }
 }
}

//_____send html answer (request without uri or argument)_______
void handleRoot()
{
 //-----increment counter, display on OLED----------------------
 requestCounter++;                          // increase counter
 display_status(3);                         // new client
 //-----send answer to client-----------------------------------
 String sHtml=String(page_answer);
 sHtml.replace("@@title@@", "Counter");
 sHtml.replace("@@body@@", String(requestCounter));
 server.send(200, "text/html", sHtml);
}

//_____software version_________________________________________
void handleVersion(){server.send(200,"text/plain","2017-07-02");}

//_____send counter value as plain answer (/plain)______________
void handlePlain()
{
 //-----increment counter, display on OLED----------------------
 requestCounter++;                          // increase counter
 display_status(3);                         // new client
 //-----send answer to client-----------------------------------
 server.send(200, "text/plain", String(requestCounter));
}

//_____reset counter and send answer (/resetcounter)____________
void handleResetCounter()
{
 requestCounter=0;
 display_status(5);                         // reset 
 //-----send answer to client-----------------------------------
 String sHtml=String(page_answer);
 sHtml.replace("@@title@@", "Counter");
 sHtml.replace("@@body@@", String(requestCounter));
 server.send(200, "text/html", sHtml);
}

//_____default config server (/config)__________________________
void handleConfigServer() { server.demo_handleConfig(); }

//_____setup routine____________________________________________
void setup(void){
 Serial.begin(9600); Serial.println("");    // init serial com
 attachInterrupt(PIN_BUTTON, doInterrupt, CHANGE);
 server.on("/", handleRoot);                //
 server.on("/version", handleVersion);      //
 server.on("/plain", handlePlain);          //
 server.on("/resetcounter", handleResetCounter);
 server.on("/config", handleConfigServer);  //
 display_status(7);                         // searching WLAN...
 server.setup();                       //check com, start server
 if(server.isConnected()) display_status(2);// Host
                     else display_status(1);// ERROR
}

//_____main loop________________________________________________
void loop(void){
 if(!server.isConnected())
 {
  display_status(1);                             // ERROR
  if(server.handleClientReconnect()) display_status(2); // Host
 }
 else
 {
  server.handleClientReconnect();
 }
}
