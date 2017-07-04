  //_____D1_oop15_basicserverkh1.ino__________170603-170603_____
// Server listens to Port 80.
// HTTP-Answer:            ok | Config not allowed | ...
// Serial monitor:         log messages (9600Bd)
// Hardware: WeMos D1 Mini

#include "D1_class_BasicserverKH.h"
BasicserverKH server(80);

//_____send answer (request without uri or argument)____________
void handleRoot() { server.demo_handleRoot(); }

//_____software version_________________________________________
void handleVersion(){server.send(200,"text/plain","2017-07-02");}

//_____send answer (request without uri or argument)____________
void handleConfigServer() { server.demo_handleConfig(); }

//_____setup routine____________________________________________
void setup(void){
 Serial.begin(9600); Serial.println("");    // init serial com
 server.on("/", handleRoot);                //
 server.on("/version", handleVersion);      //
 server.on("/config", handleConfigServer);  //
 server.setup();                       //check com, start server
 //server.setConfigWebPage(true);        // allow web page
}

//_____main loop________________________________________________
void loop(void){
 server.handleClientReconnect();
}
