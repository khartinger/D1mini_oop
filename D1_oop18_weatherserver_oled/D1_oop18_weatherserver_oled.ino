//_____D1_oop18_weatherserver_oled.ino________170611-170616_____
// Show pressure, temperature, humidity, brightness and
// wind speed (via PCF8583) on oled
// Hardware: WeMos D1 mini
//           i2c sensors: BMP180 (0x77), AM2322 (0x5C),
//                        BH1750 (0x23), and wind sensor (0x50)
#define   TITLE_  "D1_oop18_weatherserver_oled"
#include "D1_class_BasicserverKH.h"
#include "D1_class_Weathersensor1.h"
DisplayKH display_;               // display object
BasicserverKH server(80);         // 
Weathersensor1 sensor1;           //

//_____send answer (request without uri or argument)____________
void handleRoot()
{
 String s1="<h2>Usage:</h2>\n";
 s1+="/ ........this help page<br>\n";
 s1+="/config...handle ConfigServer (if enabled)<br>\n";
 s1+="/csv .....data as csv (comma separated values)<br>\n";
 s1+="/html ....web page with symbol=value unit&lt;br&gt;<br>\n";
 s1+="/json ....data as json {symbol:value,symbol:value,..}<br>\n";
 s1+="/plain ...plain text lines symbol=value unit[cr]<br>\n";
 s1+="/values...only values[cr]<br>\n";
 s1+="/version..verson of this software<br>\n";
 String sHtml=String(page_answer);
 sHtml.replace("@@title@@", TITLE_);
 sHtml.replace("@@body@@", s1);
 server.send(200, "text/html", sHtml);
}

//_____software version_________________________________________
void handleVersion(){server.send(200,"text/plain","2017-07-02");}

//_____send html config page____________________________________
void handleConfigServer() { server.demo_handleConfig(); }

//_____send answer (values only as plain text)__________________
void handleCsv()
{ server.send(200, "text/plain", sensor1.values(",")); }

//_____send html config page____________________________________
void handleHtml()
{
 String sHtml=String(page_answer);
 sHtml.replace("@@title@@", TITLE_);
 sHtml.replace("@@body@@", sensor1.toString("<br>\n"));
 server.send(200, "text/html", sHtml);
}

//_____send answer (json)_______________________________________
void handleJson()
{ server.send(200, "application/json", sensor1.json()); }

//_____send answer (plain text: symbol=value unit)______________
void handlePlain()
{ server.send(200, "text/plain", sensor1.toString("\n"," ")); }

//_____send answer (values only as plain text)__________________
void handleValues()
{ server.send(200, "text/plain", sensor1.values("\n")); }

//_____D1mini: setup and main loop______________________________
void setup() {
 display_.clearDisplay();
 server.on("/", handleRoot);                //
 server.on("/config", handleConfigServer);  //
 server.on("/csv", handleCsv);              //
 server.on("/html", handleHtml);            //
 server.on("/json", handleJson);            //
 server.on("/plain", handlePlain);          //
 server.on("/values", handleValues);        //
 server.on("/version", handleVersion);      //
 server.setup();                       //check com, start server
}

int i=10;
void loop() {
 if(++i>10)
 {
  i=0;
  sensor1.measure();
  sensor1.display(display_);
  Serial.println(sensor1.toString(" | "));
 }
 if(server.handleClientReconnect()) delay(100);
}
