# D1 mini: Einfaches MQTT Beispiel
Sketch: D1_oop68_mqtt2_blink.ino, Version 2019-12-25   
[English version](./README.md "English version")  

Der D1mini versucht sich mit dem gegebenen WLAN (SSID, Passwort) und dem MQTT-Server (Name oder IP, Port 1883) zu verbinden. Gelingt die Verbindung nicht, blinkt die blaue LED 8x kurz und der D1mini versucht erneut, sich mit dem MQTT-Server zu verbinden.   
Wenn die Verbindung erfolgreich hergestellt wurde:   
* D1mini sendet alle 2 Sekunden eine Nachricht mit dem Topic `oop68/led` und  der Payload "0" oder "1"   
* D1mini "h&ouml;rt" (subscribe) auf das selbe Topic und schaltet die blaue LED entsprechend der empfangenen Payload ein oder aus.   
* Wird die Verbindung zum Server unterbrochen, blinkt die blaue LED und der D1mini versucht, die Verbindung wieder herzustellen.   

__* Wichtig*__: Das Beispiel ben&ouml;tigt einen MQTT-Broker!!   
Die Einstellung der WLAN-Daten und des MQTT-Brokers geschieht in der Zeile
```
MqttClientKH2 client("..ssid..", "..password..","mqtt server name");   
```

## Hardware 
1. WeMos D1 mini

## Software
Die Klasse MqttClientKH2 erweitert die Klasse PubSubClient f&uuml;r eine einfache Verwendung von MQTT. Es k&ouml;nnen alle Befehle der Klasse PubSubClient verwendet werden.
Wenn die Bibliothek PubSubClient in der Arduino-IDE installiert ist, k&ouml;nnen die Dateien `PubSubClient.*` im Verzeichnis `/src/mqtt` gel&ouml;scht werden.

### Erstellen einer MQTT Anwendung
1. Setup eines Brokers  
ZB Installation von mosquitto auf einem Raspberry Pi und starten des Programms     
`sudo apt-get install mosquitto`   
`sudo apt-get install mosquitto-clients`   
`sudo /etc/init.d/mosquitto start`   

2. Erstellen eines Klienten auf den D1mini
* Klasse MqttClientKH2 einbinden   
  `#include "src/mqtt/D1_class_MqttClientKH2.h"`  
* Ein MqttClientKH2 Objekt erzeugen  
  `MqttClientKH2 client("..ssid..", "..password..","mqtt server name");`  
__*Nicht vergessen: Daten an das eigene Netzwerk anpassen!*__
* Eine **callback**-Funktion definieren, um die angemeldeten Topics zu verarbeiten.   
  `void callback(char* topic, byte* payload, unsigned int length)`  
Diese Funktion wird f&uuml;r alle angemeldeten Topics aufgerufen.   
* In der Funktion **`setup()`** wird das MQTT setup ausgef&uuml;hrt:  
  `client.addSubscribe("topic");` Anmelden eines Topics (= Input). Wird in callback() bearbeitet.   
  `client.addPublish("topic", "startvalue");` Ver&ouml;ffentlichen einer Nachricht mit dem gegebenen Topic.  
  `client.setCallback(callback);` Setzen des Namens der callback-Funktion.   
  `client.connectWiFiMQTT();` Herstellen einer Verbindung mit dem WLAN und MQTT-Server (Wartezeit max. 5 Sekunden).   
  
* In der Hauptschleife **`loop()`**  
  Regelm&auml;&szlig;iger Aufruf von `client.isConnected()`, um die zyklische Abfrage des MQTT-Servers auszuf&uuml;hren (inklusive automatischer Wiederverbindung, falls die WLAN-Verbindung unterbrochen wurde).   
