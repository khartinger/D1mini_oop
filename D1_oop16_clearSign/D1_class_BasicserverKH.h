//_____D1_class_BasicserverKH.h_____________20170605-170702_____
// D1 mini class for a simple webserver.
// This class extends class ESP8266WebServer for configurating
//  wlan data from constructor, eeprom or webpage.
//  For default values see BasicserverKH::setup_configdata()
// If you want to configure via web page, connect serial line
//  Rx and Tx on start (after reset or power on). Connecting
//  longer than 3sec sets wlan data back to constuctor values.
// server.setConfigWebPage(true); allows also configuring by
//  web page.
// D1mini blue LED on indicates no connection to WLAN (but 
//  trying to connect). When connected blue LED is off.
// This class uses class
//  Eeprom...for reading/writing D1mini internal eeprom
//  Led......for switching blue led on or off

#ifndef D1_CLASS_BASICSERVERKH_H
#define D1_CLASS_BASICSERVERKH_H
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "D1_class_Eeprom.h"
#include "D1_class_Led.h"
//-----DEFINE 1: !!! change this to your own WLAN data!!! ------
#define DEFAULT_SIGN       "BS"             //2 sign bytes EEPROM
#define DEFAULT_MDNS       "D1mini1"        //local.name for device
#define DEFAULT_SSID       "D1mini           " //17 chars (max. 20)
#define DEFAULT_PASSWORD   "12345678"       // min. 8 char or ""
#define DEFAULT_IP         192,168,1,111    // device ip
#define DEFAULT_GATEWAY    192,168,0,1      // wlan gateway
#define DEFAULT_SUBNET     255,255,248,0    // subnet mask
#define DEFAULT_HOST       192,168,1,110    // if send request
#define DEFAULT_DHCP       false            // false=no dhcp
#define HTTP_PORT          80               // 80=default for http
//-----DEFINE 2: may be changed---------------------------------
#ifndef TITLE_                         // title of web page
 #define TITLE_       "BasicserverKH"
#endif
#ifndef MAX_RESPONSE_LINES
 #define MAX_RESPONSE_LINES       20   // response from host
#endif
#define DEBUG1        true             // true=Serial output
#define BLUE_LED      true             // on ERROR: blue LED on
#define TIMEOUT_WIFI_CONNECT_MS   10000// wait for WLAN
#define TIMEOUT_CONFIG_RESET_MS   3000 // press reset button
#define TIMEOUT_RESPONSE_MS       9000 // wait for host answer
//-----DEFINE 3: text length, must fit to define 1--------------
#define ADDR_SIZE     20               // = 4 + 4ips*4bytes
#define MDNS_SIZE     20               // max.len local name
#define SSID_SIZE     20               // max.len ssid
#define PASS_SIZE     20               // max.len password
#define EEPROM_SIZE   (ADDR_SIZE+MDNS_SIZE+SSID_SIZE+PASS_SIZE)

//_____class definition_________________________________________
class BasicserverKH : public ESP8266WebServer {
 protected:
  Eeprom eeprom;                       //for eeprom access
  Led  blueled;                        // on=no WLAN
  char sign_[3];                       //sign for EEPROM
  char mdns_[MDNS_SIZE+1];             //local.name
  char ssid_[SSID_SIZE+1];             //17 chars
  char pass_[PASS_SIZE+1];             //min. 8 char or ""
  IPAddress ip, gateway, subnet, host; //wlan data
  bool dhcp;                           //true=use dhcp
  bool accesspoint;                    //true=AP, false=host
  int  port;                           //http port
  int  config_todo;                    //do config/webpage
  int  config_done;                    //status
 //-----constructors and setting of default values--------------
 public:
  BasicserverKH();
  BasicserverKH(int port);
 protected:
  void setup_constructor();
  void setup_configdata();
 //-----setter and getter methods-------------------------------
 public:
  void   setConfigWebPage(bool yes_)
         { if(yes_) config_todo|=0x10; else config_todo&=0xEF; }
  bool   isConfigWebPage() 
         { if((config_todo&0x10)>0) return true; return false; }
  bool   isConnected() { return(WiFi.status()==WL_CONNECTED); }
  String getSign()       { return String(sign_); }
  String getMDNS()       { return String(mdns_); }
  String getSSID()       { return String(ssid_); }
  String getPass()       { return String(pass_); }
  IPAddress getIP()      { return ip; }
  IPAddress getGateway() { return gateway; }
  IPAddress getSubnet()  { return subnet; }
  IPAddress getHost()    { return host; }
  int    getConfigToDo() { return config_todo; }
  int    getConfigDone() { return config_done; }
  bool   setBlueled(bool on_);         // switch blue led on/off
  bool   unsignEeprom();               // set eeprom sign to 000
 //-----demo web pages and client handle------------------------
 public:
  void demo_handleRoot();
  void demo_handleConfig();
  bool doWLAN();                       // if not connected do it
  bool handleClientReconnect();        // handleClient()+reconnect
  int  sendRequest2Host(String uri, String answer[], int lines_max);
                                       // send request to host_
  String info_connection();            // info about connection
  //-----setup methods for user program-------------------------
 public:
  void setup(bool dhcp);               // use to setup host/client
  void setupAP();                      // use to setup AP
 //-----internal methods for start and basic functions----------
 protected:
  void checkSerial(void);              // on system start
  bool checkSerial1x();                // check config button 1x
  void doConfig(void);
 //-----internal eeprom methods---------------------------------
 protected:
  bool readConfigDataFromEeprom();
  bool writeConfigDataToEeprom();
  void config_vars2wdat(uint8_t *wdat_);
  void config_wdat2vars(uint8_t *wdat_);
  uint8_t config_buildChecksum(uint8_t *data);
 //-----internal config web pages-------------------------------
 protected:
  String htmlpage1_input();
  int  config_params2wdat(uint8_t *wdat_);
  long config_checkInputByte(int nr);
};

//**************************************************************
// config web pages and parts of it
//**************************************************************

//_____setup config data (called by constructors)_______________
void BasicserverKH::setup_configdata()
{
 strcpy(sign_, DEFAULT_SIGN);          //2 chars sign for EEPROM
 strcpy(mdns_, DEFAULT_MDNS);          //local.name, max. 20
 strcpy(ssid_, DEFAULT_SSID);          //17 chars, max. 20
 strcpy(pass_, DEFAULT_PASSWORD);      //min. 8 char or ""
 ip=IPAddress(DEFAULT_IP);
 gateway=IPAddress(DEFAULT_GATEWAY);
 subnet=IPAddress(DEFAULT_SUBNET);
 host=IPAddress(DEFAULT_HOST);
 dhcp=DEFAULT_DHCP;
}

const char html_config_head[] PROGMEM=
 "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n"
 "\"http://www.w3.org/TR/html4/loose.dtd\">\n"
 "<html>\n<head>\n<title>WiFi config</title>\n"
 "<meta http-equiv=\"Content-Type\" content=\"text/html; "
 "charset=iso-8859-1\">"
 "<style>\n"
 "#in1 { width:100%; box-sizing:border-box; }\n"
 "</style>\n"
 "\n</head>\n<body>\n";

const char html_config_foot[] PROGMEM=
 "\n</body>\n</html>";

const char page1_config_input_body[] PROGMEM=
 "<h3>Config D1mini</h3>\n"
 "<form action=\"/config\" method=\"get\">\n"
 "<table border=1>\n"
 "<tr><td>IPv4-Adresse</td><td>";

const char page1_config_input_in3[] PROGMEM=
 "<input type=\"text\" size=3 maxlength=3 name=";

const char page2_config_input_ok[] PROGMEM=
 "<html><head><title>WiFi config</title></head>\n<body>\n"
 "<h3>Success</h3>Input data accepted. <br>"
 "Restart WiFi with new parameters.<br>\n"
 "<form action=\"/config\" method=\"get\">\n"
 "<input type=\"submit\" name=\"ex\" value=\"Restart\"> "
 "\n</form>\n</body></html>\n";

//**************************************************************
// constructors 
//**************************************************************

//_____constructor (default port number is 80)__________________
BasicserverKH::BasicserverKH() 
 { port=HTTP_PORT; setup_constructor(); }

//_____constructor with given port number_______________________
BasicserverKH::BasicserverKH(int port):ESP8266WebServer(port)
 { this->port=port; setup_constructor(); }

//_____setup data_______________________________________________
void BasicserverKH::setup_constructor()
{
 eeprom=Eeprom(EEPROM_SIZE);           //
 config_todo=0x90;                     // nothing to do
 config_done=0x01;                     // take data from here 
 setup_configdata();                   //
}

//**************************************************************
// setter and getter methods
//**************************************************************

//_____switch blue led on or off________________________________
bool BasicserverKH::setBlueled(bool on_)
{
 if(BLUE_LED)
 {
  if(on_) blueled.on();
     else blueled.off();
  return true;
 }
 return false;
}

//**************************************************************
// demo web pages and client handle
//**************************************************************
//_____mini web page____________________________________________
// replace @@body@@ with text
const char page_answer[] PROGMEM=
 "<html>"
 "<head><title>@@title@@</title></head>\n<body>\n"
 "@@body@@"
 "\n</body></html>\n"; 

//_____send answer to client (request without uri or argument)__
void BasicserverKH::demo_handleRoot()
{
 String sHtml=String(page_answer);
 sHtml.replace("@@title@@", TITLE_);
 sHtml.replace("@@body@@", "OK");
 send(200, "text/html", sHtml);
}

//_____config server by webpage_________________________________
void BasicserverKH::demo_handleConfig()
{
 uint8_t wdat_[EEPROM_SIZE]; 
 int webpage_;
 String sHtml;
 String s1;
 int paramnr_ex;
 int check_;
 if(DEBUG1) Serial.println("***TEST1-demo_handleConfig(): config_todo=0x"+String(config_todo,16));
 //-----is display of config web page allowed?------------------
if((config_todo&0x80)>0)
 {
  sHtml=String(page_answer);
  sHtml.replace("@@title@@", TITLE_);
  sHtml.replace("@@body@@", "Config not allowed.");
  send(200, "text/html", sHtml);  
  return;
 }
 //-----show config web pages-----------------------------------
 if(DEBUG1) Serial.println("***TEST2-demo_handleConfig(): config_todo=0x"+String(config_todo,16));
 webpage_=config_todo&0xF0;
 switch(webpage_)
 {
  case 0x10: //-----input mask----------------------------------
   sHtml=htmlpage1_input();
   send(200, "text/html", sHtml);
   config_todo|=0x30;
   break;
  
  case 0x30: //-----analyse input mask--------------------------
   // input web page: argName=ex should be last arg number
   paramnr_ex=args()-1;
   if((!argName(paramnr_ex).equals("ex")) || 
    (arg(paramnr_ex).indexOf("bernehmen")<1) )
   {
    sHtml=String(page_answer);
    sHtml.replace("@@title@@", TITLE_);
    sHtml.replace("@@body@@", "Config aborted.");
    send(200, "text/html", sHtml);  
    config_todo&=0x9F;                 //config page not allowed
    return;
   }
   //-----try to apply (web) params-----------------------------
   check_=config_params2wdat(wdat_);
   if(check_!=1)
   {
    sHtml=String(page_answer);
    sHtml.replace("@@title@@", TITLE_);
    sHtml.replace("@@body@@", "ERROR: check input #"+String(check_));
    send(200, "text/html", sHtml);  
    config_todo&=0x1F;                 // config page allowed
    return;
   }
   //-----copy (web) params to eeprom---------------------------
   config_wdat2vars(wdat_);
   if(!writeConfigDataToEeprom())
   {
    sHtml=String(page_answer);
    sHtml.replace("@@title@@", TITLE_);
    sHtml.replace("@@body@@", "ERROR writing EEPROM, config aborted.");
    send(200, "text/html", sHtml);  
    config_todo&=0x1F;                 //config page allowed
    return;
   }
   send(200, "text/html", page2_config_input_ok);
   config_todo=0x50|(config_todo&0x0F);// restart
   break;

  case 0x50: //-----restart wlan--------------------------------
   if(BLUE_LED) blueled.on();
   WiFi.disconnect();
   delay(50);
   doWLAN();
   config_todo&=0x9F;                  // config page allowed
   break;
   
  default:
   sHtml=String(page_answer);
   sHtml.replace("@@title@@", TITLE_);
   sHtml.replace("@@body@@", "Config: Error #"+String(webpage_));
   send(200, "text/html", sHtml);
   break;
 }
}

//_____check if WiFi connection is ready or make it ready_______
// dhcp=true: use dhcp
// sets blue led, if allowed (BLUE_LED=true)
// return: true connection ok, false not connected
bool BasicserverKH::doWLAN()
{
 //=====access point mode?======================================
 if(accesspoint)
 {
  //-----start WLAN---------------------------------------------
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(ssid_, pass_);
  //-----handles to answer the request--------------------------
  if(BLUE_LED) blueled.off();
  begin();
  if(DEBUG1) Serial.println("softAP "+getIP().toString()+" started.");
  return true;
 }
 //=====client/host-mode, log in to existing WLAN===============
 if(isConnected()) 
 {
  if(BLUE_LED) blueled.off();
  return true;
 }
 //-----WLAN not connected: set config data (again)-------------
 if(BLUE_LED) blueled.on();
 WiFi.disconnect();
 delay(10);
 WiFi.mode(WIFI_AP_STA); //WiFi.mode(WIFI_STA);
 if(!dhcp)
  WiFi.config(ip, gateway, subnet);         // skipp for dhcp
 WiFi.begin(ssid_, pass_);
 //-----try to connect to WLAN (access point)-------------------
 int i=TIMEOUT_WIFI_CONNECT_MS/200;
 while((!isConnected()) && (i>0))
 {
  delay(200);
  i--;
  if(DEBUG1){Serial.print("."); if(i%50==0) Serial.println("");}
 }
 //-----connected to WLAN (access point)?-----------------------
 if(i<1)
 { //-----not connected to WLAN---------------------------------
  if(DEBUG1) Serial.println("No connection - time-out!");
  return false;
 }
 //-----yes: connected to WLAN----------------------------------
 //.....start mDNS if a name is defined.........................
 if(mdns_[0]!=0) 
 { 
   if(!MDNS.begin(mdns_)) mdns_[0]=0; // if not begin: name=""
 }
 if(DEBUG1) { Serial.println("\n"+info_connection()); }
 //-----handles to answer the request---------------------------
 if(BLUE_LED) blueled.off();
 begin();
 if(DEBUG1) Serial.println("HTTP server started.");
 return true;
}

//_____handleClient_____________________________________________
// do default handleClient() = wait for a request and answer it
// return: true connection ok, false not connected
bool BasicserverKH::handleClientReconnect()
{
 //-----access point mode?--------------------------------------
 if(accesspoint)
 {
  ESP8266WebServer::handleClient();
  return true; 
 }
 //-----client/host-mode, log in to existing WLAN---------------
 bool connect_=doWLAN();
 if(!connect_) 
 {
  if(DEBUG1) Serial.println("handleClient(): No WLAN");
 }
 else
 {
  ESP8266WebServer::handleClient();
 }
 return connect_;
}

//_____send a GET request to host_______________________________
int BasicserverKH::sendRequest2Host(String uri, String answer[],
    int lines_max=MAX_RESPONSE_LINES) 
{
 int lines=0;
 WiFiClient myclient=client();
 IPAddress  myhost=getHost();
 if(!myclient.connect(myhost, HTTP_PORT)) {
  if(DEBUG1) Serial.println("ERROR: "+myhost.toString()+" connection failed!"); 
  return 0; 
 }
 //-----connected, send request---------------------------------
 String shost=myhost.toString();
 if(DEBUG1) Serial.println("-----Connected to " + shost+"-----");
 String request="GET "+ uri +  " HTTP/1.1\r\n" +
                "Host: " + shost + "\r\n" + 
                "Connection: close\r\n\r\n";
 myclient.print(request);
 delay(100);
 //-----wait for server to response with timeout----------------
 int timeout_=TIMEOUT_RESPONSE_MS/10;
 while((!myclient.available())&&(timeout_>0))
 {
  delay(10);
  timeout_--;
 }
 //-----read data, if available before timeout------------------
 if(timeout_>0)
 {
  String line="", line1="";
  bool ok_=true;
  while(myclient.available() && ok_) {
   line = myclient.readStringUntil('\r');
   line.trim();       //  remove leading and trailing whitespace
   //.....split line at \n......................................
   int len_=line.length();
   int ialt=0;
   for(int i=0; i<len_; i++)
   {
    if(line[i]=='\n')
    {
     answer[lines++]=line.substring(ialt,i);
     ialt=i+1;
     if(lines>=MAX_RESPONSE_LINES) { ok_=false; i=len_; }
    }
   }
   if(ok_) answer[lines++]=line.substring(ialt);
  }
  if(DEBUG1) Serial.println(String(lines)+" lines received");
  if(DEBUG1) Serial.println("-----connection closed--------");
 }
 else
 {
  if(DEBUG1) Serial.println("-----No Response: Timeout-----");
 }
 //-----stop client and turn LED off----------------------------
 myclient.stop();
 return lines;
}

//_____Info string about connection_____________________________
String BasicserverKH::info_connection()
{
 String s1="", cr="\r\n";
 //-----mac address---------------------------------------------
 byte mac[6];  
 WiFi.macAddress(mac);
 s1+="My MAC address: "+String(mac[5],HEX)+":";
 s1+=String(mac[4],HEX)+":"+String(mac[3],HEX)+":";
 s1+=String(mac[2],HEX)+":"+String(mac[1],HEX)+":";
 s1+=String(mac[0],HEX)+cr;
 //-----connection data-----------------------------------------
 if(isConnected())
 {
  s1+="My ip: "+getIP().toString()+cr;
  s1+="Connected to "+String(ssid_)+cr;
  int rssi = WiFi.RSSI();
  s1+="Signal strength: "+String(rssi)+" dBm received."+cr;
  if(mdns_[0]!=0)
   s1+="MDNS: local name is "+String(mdns_)+".local";
  else
   s1+="MDNS: No local name!";
 }
 else
 {
  s1+="Not connected to "+String(ssid_)+cr;
 }
 return s1;
}

//**************************************************************
// setup the server
//**************************************************************

//_____setup and start the server_______________________________
void BasicserverKH::setup(bool dhcp1=false)
{
 this->dhcp=dhcp1;
 accesspoint=false;
 //-----D1mini start: config data from constr|eeprom|webpage----
 checkSerial();
 if(DEBUG1)Serial.println("config_todo=0x"+String(config_todo,16));
 doConfig();
 //-----connect to WLAN-----------------------------------------
 doWLAN();
}

//_____setup and start the Access Point_________________________
void BasicserverKH::setupAP()
{
 accesspoint=true;
 //-----D1mini start: config data from constr|eeprom|webpage----
 checkSerial();
 if(DEBUG1)Serial.println("config_todo=0x"+String(config_todo,16));
 doConfig();
 //-----start WLAN----------------------------------------------
 doWLAN();
}

//_____check, if lines Tx and Rx are connected__________________
// If lines Tx and Rx are connected, get config data from...
// not connected: ...eeprom                (config_todo=0x02)
// <3s connected: ...eeprom, then web page (config_todo=0x12)
// >3s connected: ...constr, then web page (config_todo=0x11)
// "return": config_todo says what to do
// calls checkSerial1x() more times
// called by setup()
void BasicserverKH::checkSerial()
{
 bool config_key=true;
 //-----check, if Serial is ready-------------------------------
 if(!Serial)
 {
  Serial.begin(9600);
  delay(10);
  if(!Serial) { config_todo=0x02; return; } // Serial not ready
 }
 //-----check first time for config button pressed--------------
 Serial.println(); delay(2);      // 1. Zeilenumbruch
 config_key = checkSerial1x();
 if(!config_key) 
 {
  if(DEBUG1)Serial.println("==standard mode: ip from eeprom===");
  // config_todo=0x02;  // old: disable web page, read eeprom
  config_todo=0x92;     // disable web page, read eeprom  
  delay(1000);
  return;
 }
 //-----loop: test every 0,25sec if button is still pressed-----
 int i=TIMEOUT_CONFIG_RESET_MS/250;
 do
 {
  i--;
  delay(230);                     // checkSerial1x() waits 20ms
  config_key=checkSerial1x();
 } while(config_key && (i>0));
 //-----which end of loop?--------------------------------------
 if(i<=0)
 {
  if(DEBUG1)Serial.println("==RESET: ip from constr.,webpage==");
  config_todo=0x19; //enable web page, read constr, write eeprom
 }
 else
 {
  if(DEBUG1)Serial.println("==CONFIG: ip from eeprom,webpage==");  
  config_todo=0x12; //enable web page, read eeprom
 }
}

//_____check, if lines Tx and Rx are connected__________________
// send teststring one time and check, if received it back
// called by checkSerial()
// return: false=NO key pressed, true=config key pressed
bool BasicserverKH::checkSerial1x()
{
 bool config_key=true;
 //-----clean buffers-------------------------------------------
 Serial.flush();                  // wait for empty outgoing buf
 while(Serial.available())        // + empty incomming
 {
  Serial.read();                  // + buffer
  delay(1);
 }
 //-----test, if config is wanted-------------------------------
 String teststring="config";      
 int len_=teststring.length();    // length = 6
 Serial.println(teststring);      // send test string
 delay(20);                       // wait for answer
 Serial.flush();                  // wait for empty outgoing buf
 int anz=Serial.available();      // characters received?
 if(anz!=len_+2)                  // teststring + \r\n
  config_key=false;               // nothing/wrong received
 else
 {
  char answer_[len_+2];           // len_+2 = 8
  Serial.readBytes(answer_,len_); // 6="config".length();
  answer_[len_]=0;                // ad 0 = end of string
  String s2=String(answer_);      // convert to string
  if(!teststring.equals(s2))      // received string ok?
   config_key=false;              // wrong test string received
 }
 return config_key;
}

//_____get config data as in config_todo is requested___________
// pkks w-ec
// p...1 = do not show web page
// k...kind of web page
// s...1= server answer allowed (must be 1, bug)
// w...1 = write config to eeprom
// -...bit not used
// e...1 = read config data from eeprom
// c...1 = read config data from constructor
void BasicserverKH::doConfig()
{
 bool disconnect=false;
 //-----read config data from constructor-----------------------
 if((config_todo&0x01)>0) {
  setup_configdata();
  config_done|=0x01;
  disconnect=true;
 }
 //-----read config data from eeprom----------------------------
 if((config_todo&0x02)>0) {
  if(readConfigDataFromEeprom()) config_done|=0x02;
                            else config_done&=0xFD;
  disconnect=true;
 }
 //-----write config data to eeprom-----------------------------
 if((config_todo&0x08)>0) {
  if(writeConfigDataToEeprom()) config_done|=0x20;
                           else config_done&=0xDF;
 }
 //-----disconnect, if config data changed----------------------
 if(disconnect)
 {
  if(BLUE_LED) blueled.on();
  if(isConnected()) 
  {
   if(DEBUG1) Serial.println("Disconnect old WiFi connection.");
   WiFi.disconnect();
   delay(500);
  }
  else
  {
   if(DEBUG1) Serial.println("Not connected to any WiFi.");
  }
 }
} 

//**************************************************************
// eeprom methods
//**************************************************************

//_____set first 3 eeprom bytes to zero_________________________ 
// return: true=ok, false=error
bool BasicserverKH::unsignEeprom()
{
 int anz=0;
 uint8_t data_[3]={0,0,0};
 if(eeprom.writeBlock(data_,0,3)!=3) return false;
 if(eeprom.readBlock (data_,0,3)!=3) return false;
 if((data_[0]+data_[1]+data_[2])!=0) return false;
 return true;
}

//_____read data from eeprom____________________________________
// return: true=ok, false=error
bool BasicserverKH::readConfigDataFromEeprom()
{
 uint8_t wdat_[EEPROM_SIZE];
 //-----reading data from eeprom done or tried to do------------
 config_todo=config_todo&0xFD;        // clear eeprom read bit
 //-----read block and check data-------------------------------
 bool ok=false;
 int anz=eeprom.readBlock(wdat_, 0, EEPROM_SIZE);
 if(anz==EEPROM_SIZE)
 {//.....check sign.............................................
  if(wdat_[0]==sign_[0]&&wdat_[1]==sign_[1]&&wdat_[2]==sign_[2])
  {
   int checksum=config_buildChecksum(wdat_);
   if(checksum==wdat_[3]) ok=true;
  }
 }
 //.....everything ok?.......................................... 
 if(!ok)
 {
  config_done=config_done&0xFD;        // no eeprom data 
  return false; 
 }
 //-----data ok -> copy to vars---------------------------------
 config_wdat2vars(wdat_);
 //-----data from eeprom----------------------------------------
 config_done=config_done|0x02;         // data from eeprom done
 return true;
}

//_____write data to eeprom_____________________________________
// return: true=ok, false=error
bool BasicserverKH::writeConfigDataToEeprom()
{
 uint8_t wdat_[EEPROM_SIZE];
 uint8_t wdat_in[EEPROM_SIZE];
 config_vars2wdat(wdat_);
 //-----write block and check data------------------------------
 bool ok=false;
 int anz=eeprom.writeBlock(wdat_, 0, EEPROM_SIZE);
 if(anz==EEPROM_SIZE)
 {
  int temp=config_done;                // data will not be...
  anz=eeprom.readBlock(wdat_in, 0, EEPROM_SIZE);
  config_done=temp;                    // ...copied to vars
  if(anz==EEPROM_SIZE)
  {
   //.....are all written and read bytes equal?.................
   bool ok1=true;
   for(int i=0; i<anz; i++)
   {
    if(wdat_[i]!=wdat_in[i]) { ok1=false; break; }
   }
   ok=ok1;
  }
 }
 //.....everything ok?..........................................
 if(!ok)
 {
  unsignEeprom();                      // eeprom data invalid
  config_done=config_done&0xF7;        // not written to eeprom
  return false; 
 }
 //-----data written to eeprom----------------------------------
 config_done=config_done|0x80;         // data written to eeprom
 return true;
}

//_____transfer wifi data from vars to array____________________
void BasicserverKH::config_vars2wdat(uint8_t *wdat_)
{
 //.....set addresses...........................................
 wdat_[4]=ip[0];       wdat_[5]=ip[1];
 wdat_[6]=ip[2];       wdat_[7]=ip[3];
 wdat_[8] =subnet[0];  wdat_[9] =subnet[1];
 wdat_[10]=subnet[2];  wdat_[11]=subnet[3];
 wdat_[12]=gateway[0]; wdat_[13]=gateway[1];
 wdat_[14]=gateway[2]; wdat_[15]=gateway[3];
 wdat_[16]=host[0];    wdat_[17]=host[1];
 wdat_[18]=host[2];    wdat_[19]=host[3];
 //.....set mDNS name.local.....................................
 for(int i=0; i<MDNS_SIZE; i++)
  wdat_[ADDR_SIZE+i]=mdns_[i];
 //.....set ssid................................................
 for(int i=0; i<SSID_SIZE; i++)
  wdat_[ADDR_SIZE+MDNS_SIZE+i]=ssid_[i];
 //.....set password............................................
 for(int i=0; i<PASS_SIZE; i++)
  wdat_[ADDR_SIZE+MDNS_SIZE+SSID_SIZE+i]=pass_[i];
 //.....first 4 values..........................................
 wdat_[0]=sign_[0];               // 'S';
 wdat_[1]=sign_[1];               // 'V';
 wdat_[2]=0;
 uint8_t checksum=config_buildChecksum(wdat_);
 wdat_[3]=checksum;
}

//_____transfer wifi data from array to vars____________________
void BasicserverKH::config_wdat2vars(uint8_t *wdat_)
{
 //.....set addresses...........................................
 ip     =IPAddress(wdat_[4],  wdat_[5],  wdat_[6],  wdat_[7]);
 subnet =IPAddress(wdat_[8],  wdat_[9],  wdat_[10], wdat_[11]);
 gateway=IPAddress(wdat_[12], wdat_[13], wdat_[14], wdat_[15]);
 host   =IPAddress(wdat_[16], wdat_[17], wdat_[18], wdat_[19]);
 //.....set mDNS name.local.....................................
 for(int i=0; i<MDNS_SIZE; i++)
  mdns_[i]=wdat_[ADDR_SIZE+i];
 //.....set ssid_...............................................
 for(int i=0; i<SSID_SIZE; i++)
  ssid_[i]=wdat_[ADDR_SIZE+MDNS_SIZE+i];
 //.....set pass_...............................................
 for(int i=0; i<PASS_SIZE; i++)
  pass_[i]=wdat_[ADDR_SIZE+MDNS_SIZE+SSID_SIZE+i];
}

//_____build checksum___________________________________________
// return: checkbyte
uint8_t BasicserverKH::config_buildChecksum(uint8_t *data)
{
 int i;
 uint8_t checksum=0;
 for(i=4; i<EEPROM_SIZE; i++) 
  checksum=checksum^data[i];
 return checksum;
}

//**************************************************************
// config web pages
//**************************************************************
//_____build web page with actual values________________________
String BasicserverKH::htmlpage1_input()
{
 uint8_t wdat_[EEPROM_SIZE];
 config_vars2wdat(wdat_);
 //-----convert default names to string-------------------------
 char c;
 String nloc_="";
 int i=0, i0=ADDR_SIZE;
 for(i=0; i<MDNS_SIZE; i++)
 { c=(char)wdat_[i0+i]; if(c!=0) nloc_+=c;
 }
 String ssid_="";
 i0=i0+i;
 for(i=0; i<SSID_SIZE; i++)
 { c=(char)wdat_[i0+i]; if(c!=0) ssid_+=c;
 }
 String pwd_="";
 i0=i0+i;
 for(i=0; i<PASS_SIZE; i++)
 { c=(char)wdat_[i0+i]; if(c!=0) pwd_+=c;
 }
 //-----build web page------------------------------------------
 String s1="";
 s1+=String(html_config_head);
 s1+=String(page1_config_input_body);
 s1+=String(page1_config_input_in3);
 s1+="\"i0\" value=\""+String(wdat_[4])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"i1\" value=\""+String(wdat_[5])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"i2\" value=\""+String(wdat_[6])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"i3\" value=\""+String(wdat_[7])+"\">";
 s1+="</td></tr>\n<tr>";
 s1+="<td>Subnetzmaske</td><td>";
 s1+=String(page1_config_input_in3);
 s1+="\"m0\" value=\""+String(wdat_[8])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"m1\" value=\""+String(wdat_[9])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"m2\" value=\""+String(wdat_[10])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"m3\" value=\""+String(wdat_[11])+"\">";
 s1+="</td></tr>\n<tr>";
 s1+="<td>Gateway</td><td>";
 s1+=String(page1_config_input_in3);
 s1+="\"g0\" value=\""+String(wdat_[12])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"g1\" value=\""+String(wdat_[13])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"g2\" value=\""+String(wdat_[14])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"g3\" value=\""+String(wdat_[15])+"\">";
 s1+="</td></tr>\n";
 s1+="<td>Host</td><td>";
 s1+=String(page1_config_input_in3);
 s1+="\"h0\" value=\""+String(wdat_[16])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"h1\" value=\""+String(wdat_[17])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"h2\" value=\""+String(wdat_[18])+"\">.";
 s1+=String(page1_config_input_in3);
 s1+="\"h3\" value=\""+String(wdat_[19])+"\">";
 s1+="</td></tr>\n";
 //-----text inputs---------------------------------------------
 s1+="<tr><td>mDNS: Local name &nbsp; </td><td>";
 s1+="<input id=\"in1\" type=\"text\" size=20 maxlength=20 ";
 s1+="name=\"n\" value=\""+nloc_+"\"></td></tr>\n";
 s1+="<tr><td>ssid_</td><td>";
 s1+="<input id=\"in1\" type=\"text\" size=20 maxlength=20 ";
 s1+="name=\"s\" value=\""+ssid_+"\"></td></tr>\n";
 s1+="<tr><td>Passwort</td><td>";
 s1+="<input id=\"in1\" type=\"password\" size=20 maxlength=20 ";
 s1+="name=\"p\" value=\""+pwd_+"\"></td></tr>\n</table>\n<br>\n";
 //-----buttons-------------------------------------------------
 s1+="<input type=\"submit\" name=\"ex\" value=\"Abbrechen\"> ";
 s1+="&nbsp; \n";
 s1+="<input type=\"submit\" name=\"ex\" value=\"\r &Uuml;bernehmen!\">";
 s1+=String(html_config_foot);
 return s1;
}

//_____check if part of input string is value from 0..255_______
// called by config_params2wdat()
// return: 0..255 if ok, <0 on error
long BasicserverKH::config_checkInputByte(int nr)
{
 long t1_;
 String s1=arg(nr);
 for(int i=0; i<s1.length(); i++)
 {
  if(!isDigit(s1.charAt(i))) return -((nr+1)*10+i+2);
 }
 t1_=s1.toInt();
 if((t1_<0)||(t1_>255)) return -((nr+1)*10+1);
 return t1_;
}

//_____check webpage params and copy them to wdat_______________
// return: 1=data ok, <0 error
int BasicserverKH::config_params2wdat(uint8_t *wdat_)
{
 int i, i0=0, len_=0;
 long t1_;
 String s1;
 uint8_t tmp_[EEPROM_SIZE];
 //----first 4 values-------------------------------------------
 tmp_[i0++]=sign_[0];             //'S';
 tmp_[i0++]=sign_[1];             //'V';
 tmp_[i0++]=0;
 tmp_[i0++]=0;
 //-----check IP input------------------------------------------
 for(i=0; i<4; i++)
 {
  s1="i"+String(i);
  if(!argName(i).equals(s1)) return -(i+1)*10;
  t1_=config_checkInputByte(i); if(t1_<0) return t1_;
  tmp_[i0++]=t1_;
//  Serial.println("check ok");
 }
 //-----check subnetmask input----------------------------------
 for(i=0; i<4; i++)
 {
  s1="m"+String(i);
  if(!argName(i+4).equals(s1)) return -(i+5)*10;
  t1_=config_checkInputByte(i+4); if(t1_<0) return t1_;
  tmp_[i0++]=t1_;
 }
 //-----check gateway input-------------------------------------
 for(i=0; i<4; i++)
 {
  s1="g"+String(i);
  if(!argName(i+8).equals(s1)) return -(i+9)*10;
  t1_=config_checkInputByte(i+8); if(t1_<0) return t1_;
  tmp_[i0++]=t1_;
 }
 //-----check host input----------------------------------------
 for(i=0; i<4; i++)
 {
  s1="h"+String(i);
  if(!argName(i+12).equals(s1)) return -(i+13)*10;
  t1_=config_checkInputByte(i+12); if(t1_<0) return t1_;
  tmp_[i0++]=t1_;
 }
 //-----check mDNS (local name)---------------------------------
 if(!argName(16).equals("n")) return -120;
 s1=arg(16);
 len_=s1.length();
 if(len_>MDNS_SIZE) return -121;
 for(i=0; i<len_; i++) tmp_[i0++]=s1[i];
 for(i=len_; i<MDNS_SIZE; i++) tmp_[i0++]=0;
 //-----check ssid----------------------------------------------
 if(!argName(17).equals("s")) return -130;
 s1=arg(17);
 len_=s1.length();
 if(len_>SSID_SIZE) return -131;
 for(i=0; i<len_; i++) tmp_[i0++]=s1[i];
 for(i=len_; i<SSID_SIZE; i++) tmp_[i0++]=0;
 //-----check password------------------------------------------
 if(!argName(18).equals("p")) return -140;
 s1=arg(18);
 len_=s1.length();
 if(len_>PASS_SIZE) return -141;
 for(i=0; i<len_; i++) tmp_[i0++]=s1[i];
 for(i=len_; i<PASS_SIZE; i++) tmp_[i0++]=0; 
 //.....build and save checksum.................................
 tmp_[3]=config_buildChecksum(tmp_);
 Serial.println("getParams: checksum="+String(tmp_[3]));
 //-----copy input to parameter---------------------------------
 for(i=0; i<EEPROM_SIZE; i++)
  wdat_[i]=tmp_[i];
 return 1;
}

#endif

