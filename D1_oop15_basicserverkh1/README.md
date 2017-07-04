# D1_oop15_basicserverkh1 
This example implements a simple server (ip 192.168.1.100, port 80) using the class __BasicserverKH__. A request is answered by "OK" or "Config not allowed" or "xxx not found".
The D1mini blue LED indicates the state of WLAN:
* blue LED on:  no connection to WLAN (but trying to connect)
* blue LED off: D1 mini connected to WLAN

if DEBUG1 is true, status messages are shown on Serial (9600Baud).

## Changing wlan data
Default WLAN values are defined at the beginning of file D1_class_BasicserverKH.h and must be changed to your own values.
```c++
#define DEFAULT_SIGN       "BS"             //2 sign bytes EEPROM
#define DEFAULT_MDNS       "D1mini1"        //local.name for device
#define DEFAULT_SSID       "D1mini           " //17 chars (max. 20)
#define DEFAULT_PASSWORD   "12345678"       // min. 8 char or ""
#define DEFAULT_IP         192,168,1,100    // device ip
#define DEFAULT_GATEWAY    192,168,0,1      // wlan gateway
#define DEFAULT_SUBNET     255,255,248,0    // subnet mask
#define DEFAULT_HOST       192,168,1,111    // if send request
#define DEFAULT_DHCP       false            // false=no dhcp
#define HTTP_PORT          80               // 80=default for http
```
After programming the D1 mini, default wlan values are stored in the EEPROM. On a reset/start the wlan values are read from eeprom. When pins Rx and Tx from the external serial line are connected on start (after reset or power on), you can change WLAN values calling the web page `/config`. Connecting Tx-Rx longer than 3sec sets WLAN data back to default values.

The code line `server.setConfigWebPage(true);` in your program allows configuring by web page as well (no Tx-Rx connection needed on start).

The following defines may be changed:
```c++
#define TITLE_       "BasicserverKH"
#define MAX_RESPONSE_LINES       20   // response from host
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
```

## Hardware
* WeMos D1 mini


## Class BasicserverKH
The class BasicserverKH extends the class ESP8266WebServer and uses objects of the classes Eeprom and Led.

| --- *properties* --- |     |
| -------------------- | --- |
| ~ Eeprom eeprom | object for eeprom access |
| ~ Led  blueled | LED on = no WLAN access |
| ~ char sign_[3] | 2 sign bytes for EEPROM data |
| ~ char mdns_[MDNS_SIZE+1] | local.name for the device |
| ~ char ssid_[SSID_SIZE+1] | 17 chars (max. 20) |
| ~ char pass_[PASS_SIZE+1] | password (min. 8 char or "") |
| ~ IPAddress ip, gateway, subnet, host | WLAN data |
| ~ bool dhcp | true=use dhcp |
| ~ bool accesspoint | true=D1 mini is access point, false=host |
| ~ int port | http port (default 80) |
| ~ int  config_todo | do config/webpage, config data from where |
| ~ int  config_done | status |
| ~ IPAddress lastClient | IP of last client that was connected |

| <nobr>--- *constructors* (and helpers) ---</nobr> |     |
| ------------------------------------ | --- |
| \+ __BasicserverKH__()               | Make a BasicserverKH-object, set port to 80. |
| \+ __BasicserverKH__(int port)       | Make a BasicserverKH-object for the given port. |
| ~ void setup_constructor()           | Called by constuctors. Defaults for eeprom, config_todo and config_done. Calls setup_configdata().  |
| ~ void setup_configdata()            | Set defaults (from `#define`) for sign_, mdns_, ... |

| --- *setup methods for user program* --- |   |
| --- | --- |
| \+ void __setup__(bool dhcp) | use this to setup a client or host    |
| \+ void __setupAP__()        | use this to setup a access point    |

| --- *demo web pages and client handle* --- |     |
| ------------------------------------------ | --- |
| \+ void __demo_handleRoot__()   | send answer OK to client (if request without uri or argument) |
| \+ void __demo_handleConfig__() |   |
| \+ bool __doWLAN()__() | if not connected to WLAN: do it |
| \+ void __handleClientReconnect__() | AP: call handleClient() from ESP8266WebServer. Host: If not connected -> try to reconnect, then call handleClient() |
| \+ int __sendRequest2Host__(String uri, String answer[], int lines_max) | send a GET request to host |
| \+ String __info_connection__() | info about connection |

| --- *setter and getter methods* ---     |     |
| --------------------------------------- | --- |
| \+ void __setConfigWebPage__(bool yes_) |     |
| \+ bool __isConfigWebPage__() |   |
| \+ bool __isConnected__() |   |
| \+ bool __isClient__() |   |
| \+ String __getSign__() |   |
| \+ String __getMDNS__() |   |
| \+ String __getSSID__() |   |
| \+ String __getPass__() |   |
| \+ IPAddress __getIP__() |   |
| \+ IPAddress __getGateway__() |   |
| \+ IPAddress __getSubnet__() |   |
| \+ IPAddress __getHost__() |   |
| \+ int __getConfigToDo__() |   |
| \+ int __getConfigDone__() |   |

| --- *internal methods for start and basic functions* --- |     |
| -------------------------------------------------------- | --- |
| ~ void checkSerial()   | called on D1 mini start by setup() or setupAP() |
| ~ bool checkSerial1x() | called by checkSerial(). Checks, if lines Tx and Rx are connected: Send a teststring and try receive it |
| ~ void doConfig()      | get config data from where in config_todo is set |

| --- *eeprom methods* --- |     |
| ------------------------ | --- |
| \+ bool __unsignEeprom__() | set first 3 eeprom bytes to zero  |
| ~ bool readConfigDataFromEeprom() | read WLAN config data from EEPROM. Return: true=ok |
| ~ bool writeConfigDataToEeprom() | write WLAN config data into EEPROM. Return: true=ok  |
| ~ void config_vars2wdat(uint8_t *wdat_) | helper: transfer wifi data from vars to array |
| ~ void config_wdat2vars(uint8_t *wdat_) | helper: transfer wifi data from array to vars |
| ~ uint8_t config_buildChecksum(uint8_t *data) | build checksum of given data |

| --- *internal config web pages* --- |     |
| ----------------------------------- | --- |
| ~ String htmlpage1_input() | build web page with actual values, return it as String |
| ~ long config_checkInputByte(int nr) | check if part of input string is value from 0..255 |
| ~ int  config_params2wdat(uint8_t *wdat_) | check webpage params and copy them to wdat |

