//_____D1_class_TimeHttp.h____________________171127-171127_____
// The class TimeHttp sends a HTTP 1.1 request (port 80) with 
// missing 'Host: ' in header to a server and waits for the 
// error answer that includes a 'Date: ' header ;)
// Created by Karl Hartinger, November 27, 2017.
// Released into the public domain.
#ifndef D1_CLASS_TIMEHTTP_H
#define D1_CLASS_TIMEHTTP_H
#include "Arduino.h"    
#include <ESP8266WiFi.h>
#define SSID_SIZE    20                // max.len ssid
#define PASS_SIZE    20                // max.len password
#define HTTP_SIZE    20                // max.len http servername
#define HTTP_SERVER   "192.168.1.1"    // ip of http server
#define PORT_        80                // default http port
#define HTTP_RECONNECT_MS         4000 // time between reconnect
#define TIMEOUT_WIFI_CONNECT_MS   8000 // max. waiting for WLAN
#ifndef DEBUG1
 #define DEBUG1              true      // true=Serial output
#endif

class TimeHttp {
 protected:
 //-----struct for date, time, day of week UTC!-----------------
 struct Dati                      // struct for date and time
 {
  int  yy;                        // year (1970...) 
  byte mm, dd;                    // month (1..12), day (1..31)
  byte HH, MM, SS;                // hour (0..23),minute,seconds
  byte ww;                        // day of week 0=sunday, ..
 };
 //-----properties----------------------------------------------
 protected:
  char ssid_[SSID_SIZE+1];        // ssid (text)
  char pass_[PASS_SIZE+1];        // password (text)
  char http_[HTTP_SIZE+1];        // http server name
  int  port_;                     // http port (default: PORT_)
  WiFiClient d1miniClient;        // WLAN client
  long millis_lastConnected;      // last connect time [ms]
  struct Dati dt_;                // struct for date and time
 public:
  //-----constructor--------------------------------------------
  TimeHttp(char* ssid, char* pwd);
  TimeHttp(char* ssid, char* pwd, char* http_server);
  TimeHttp(char* ssid, char* pwd, char* http_server, unsigned int port);
 protected:
  void   setup();                 // setup properties
 public:
  //-----methods to setup WLAN----------------------------------
  bool   setup_wifi();            //
  bool   reconnect();             //
  bool   isConnected();           //
  //-----methods for http request-------------------------------
  int    sendRequest();           //
  String getDateTime();           //
  String getDateTime2();          //
  //-----little helpers-----------------------------------------
  bool   isLeapyear(int year);
  byte   getDayOfWeek(int yyyy, byte mm, byte dd);
  String addsubHours(int yy, int mm, int dd, 
         int HH, int MM, int SS, int hours, int style);
  String addsubHours(String sDT, int hours);
  String addsubHours2(String sDT, int hours);
  int    getSummertime(int yy, int mm, int dd, 
         int HH, int timezone);
  int    getSummertime(String sDT_UTC, int timezone);
  int    getSummertime2(String sDT_UTC, int timezone);
  //-----convert struct date <-> seconds since 1.1.1970---------
 protected:
  unsigned long dati2sec(struct Dati dt1);
  void   sec2dati(unsigned long sec1970, struct Dati* dt1,
         int timezone);
  void   addsubHours(struct Dati* dt1, int hours);
};
#endif
