//_____D1_oop17_basicServerClientAP.ino_______170626-170626_____
// One D1 mini works as access point, host and client
// * access point: generate WLAN.
// * host/server : a request to / is answered by "AP: ip="+ip
// * client      : when button D3 is pressed, a request is sent
//                 to host, answer is sent to Serial (9600Bd)
// Hardware: WeMos D1 Mini
//           1-BUTTON Shield
#include "D1_class_BasicserverKH.h"
#include "D1_class_Din.h"
BasicserverKH server(80);
Din button(D3);
String sResponse[MAX_RESPONSE_LINES];

//_____work as host (server): send answer (plain text)__________
void handleRoot() {
 String s1="AP: ip="+server.getIP().toString();
 server.send(200, "text/plain", s1);
}

//_____software version_________________________________________
void handleVersion(){server.send(200,"text/plain","2017-07-02");}

//_____default config server (/config)__________________________
void handleConfigServer() { server.demo_handleConfig(); }

//_____setup routine____________________________________________
void setup(void){
 Serial.begin(9600); Serial.println("");    // init serial com
 server.on("/", handleRoot);                // for host
 server.on("/version", handleVersion);      //
 server.on("/config", handleConfigServer);  //
 server.setupAP();
}

//_____main loop________________________________________________
void loop(void){
 //-----wait for client-----------------------------------------
 server.handleClientReconnect();
 //-----work as client: button pressed?-------------------------
 if(button.is_falling_edge())
 {
  Serial.println("Button pressed!");
  int lines=server.sendRequest2Host("/", sResponse);
  Serial.println("-----loop: Show Response------");
  for(int i=0; i<lines; i++) 
   Serial.println(String(i+1)+": "+sResponse[i]);
  Serial.println("-----(End of Response)--------");
 }
}
