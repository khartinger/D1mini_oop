# D1 mini: Schalten einer Lampe &uuml;ber ein Relais mit MQTT
Version 2020-01-10 D1_oop42_mqtt_relais1.ino    
[English version](./README.md "English version")   

Dieses Beispiel verwendet einen D1 mini als MQTT Client zum Schalten einer Lampe &uuml;ber ein Relais und zum Messen des Stromes. Wenn der D1 mini ein Topic `relay1/set/relay` mit dem Inhalt (Payload) `on` empf&auml;ngt, schaltet er das Ralais ein, mit `off` schaltet er das Relais aus und mit `toggle` um.   
Die beiden Leds dienen dazu, den Systemstatus anzuzeigen:   
* DUO-LED rot blinkend: Nicht mit WLAN verbunden
* DUO-LED rot: MQTT Fehler
* DUO-LED gr&uuml;n: MQTT Verbindung zum Broker OK. Die gr&uuml;ne Led blinkt alle 2,5 Sekunden kurz auf, um anzuzeigen, dass der D1 mini funktioniert.   

__*Nicht vergessen: Die WLAN-Daten an das eigene Netzwerk anpassen*__ in der Zeile:   
`MqttClientKH2 client("..ssid..", "..password..","mqtt server name");`  

![D1mini mit INA122- und Relais-Shield](./images/D1_mqtt_relais1_2.png "D1mini mit INA122- und Relais-Shield")    
_Bild 1: D1mini mit INA122- und Relais-Shield_   

### Hardware
1. WeMos D1 mini   
2. Relais Shield   
3. Selbstbau-Shield D1_INA122_V3_191108 (oder D1_INA122_V2_180924) oder Stromwandler (zB ASM-010) am Analog-In-Eingang   
4. (gr&uuml;ne) LED an D8, Duo-LED an D6 (gr&uuml;n)/D7 (rot)   

### Details zu den MQTT-Nachrichten   
* Alle Topics f&uuml;r MQTT-Nachrichten an den D1 mini beginnen mit `relay1/`   
* Alle MQTT-__Anfragen__ verwenden das selbe Topic `relay1/get`, der Typ der gew&uuml;nschte Information wird in der Payload angegeben.   
*M&ouml;gliche Payloads sind im Array sGet[] definiert.*   
* Antworten auf Anfragen werden unter dem Topic `relay1/ret/[sGet]` versendet, wobei [sGet] den Typ von Information darstellt. Die Payload enth&auml;lt das Ergebnis.   
* Befehle an den D1 mini verwenden das Topic `relay1/set/[sSet]`, wobei [sGet] den Typ von Information darstellt. Die Payload enth&auml;lt den einzustellenden Wert.   
*M&ouml;gliche Payloads sind im Array sSet[] definiert.*   

__*Beispiele*__  
* Das Topic `relay1/get` mit Inhalt (Payload) `help` ergibt die Antwort Topic `relay1/ret/help` mit dem Inhalt (Payload) `get: help|version|function|ip|all|relay|current|current0|
set: relay|current0|`
* Das Topic `relay1/set/relay` mit Inhalt (Payload) `on` ergibt die Antwort Topic `relay1/ret/relay` mit dem Inhalt (Payload) `1` und schaltet das Relais ein.

### Test mit Mosquitto Publisher und Subscriber
1. Mit dem Wifi (WLAN) verbinden in dem sich der MQTT Server befindet.
2. In Windows oder Linux ein Konsolenfenster aufmachen und alle Nachrichten f&uuml;r den D1mini anzeigen (subscribe, anfordern):   
`mosquitto_sub -h 10.1.1.1 -t "relay1/#" -v`  
3. In Windows oder Linux ein zweites Konsolenfenster aufmachen.   
Das Relais einschalten mit    
`mosquitto_pub -h 10.1.1.1 -t "relay1/set/relay" -m on`  
Das Relais ausschalten mit    
`mosquitto_pub -h 10.1.1.1 -t "relay1/set/relay" -m off`  
Das Relais umschalten mit    
`mosquitto_pub -h 10.1.1.1 -t "relay1/set/relay" -m toggle`  

### Software
Dieses Besispiel verwendet die folgenden Dateien (Klassen):   
* `PubSubClient` und `D1_class_MqttClientKH2` (siehe Verzeichnis `src\mqtt2`)   
* `D1_class_Relay1` und `D1_class_Ain` (siehe Verzeichnisse `src\relay1` und `src\ain`)   
* `D1_class_Statemachine` (siehe Verzeichnis `src\statemachine`)   