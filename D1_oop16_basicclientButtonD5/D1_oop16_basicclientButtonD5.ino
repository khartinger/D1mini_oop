//_____D1_oop16_basicclientButtonD5.ino_______170616-170702_____
// Client: Call host, when button D5 is pressed, and print the
//         answer (response) to Serial (9600 Baud)
// Configuration Server: listens to Port 80, when allowed
//         HTTP-Answer: ok | Config not allowed | ... not found
// Hardware: WeMos D1 Mini
#define MAX_RESPONSE_LINES        15   // response from host
#include "D1_class_BasicserverKH.h"
#include "D1_class_Din.h"
BasicserverKH server(80);
Din button(D5);
String sResponse[MAX_RESPONSE_LINES];

//**************************************************************
// Basicserver functions for WLAN configuration via web page
//**************************************************************

//_____send html answer (request without uri or argument)_______
void handleRoot() { server.demo_handleRoot(); }

//_____software version_________________________________________
void handleVersion(){server.send(200,"text/plain","2017-07-02");}

//_____default config server (/config)__________________________
void handleConfigServer() { server.demo_handleConfig(); }

//_____setup routine____________________________________________
void setup(void){
 Serial.begin(9600); Serial.println("");    // init serial com
 //-----web page config allowed? Yes: install config server-----
 server.on("/", handleRoot);                //
 server.on("/version", handleVersion);      //
 server.on("/config", handleConfigServer);  //
 server.setup();                      //check com, start server
}

//_____main loop________________________________________________
void loop(void){
 //-----if config allowed: do default handleClient()------------
 if(server.isConfigWebPage()) {server.handleClientReconnect();}
 //-----button pressed?-----------------------------------------
 if(button.is_falling_edge())
 {
  Serial.println("Button pressed!");
  //int lines=server.sendRequest2Host("/plain", sResponse);
  int lines=server.sendRequest2Host("/", sResponse);
  Serial.println("-----loop: Show Response------");
  for(int i=0; i<lines; i++) 
   Serial.println(String(i+1)+": "+sResponse[i]);
  Serial.println("-----(End of Response)--------");
 }
}
