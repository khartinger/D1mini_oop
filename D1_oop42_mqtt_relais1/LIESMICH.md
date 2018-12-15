# D1 mini: Schalten eines Relais mit MQTT
Version 2018-11-11, Datei: `D1_oop42_mqtt_relais1.ino`   
[English version](./README.md "English version")   

Dieses Beispiel zeigt das Schalten einer Lampe (auch 230V) durch einen D1 mini mit Relais-Shield und MQTT Nachrichten.
Zus&auml;tzlich misst der D1 mini den Wechselstrom mit Hilfe eines Stromwandlers ASM-010 und einem selbstgebauten INA122-Verst&auml;rker-Shield.   

__Wichtig__: Dieser Sketch ben&ouml;tigt einen Broker, zB einen Raspberry Pi als Access Point f&uuml;r ein WLAN `Raspi10` mit dem Passwort `12345678`, einer statischen Adresser `10.1.1.1` und installiertem Mosquitto. Siehe [https://github.com/khartinger/Raspberry-as-mqtt-broker](https://github.com/khartinger/Raspberry-as-mqtt-broker "Raspberry pi as broker")   
__*Anpassung der WiFi-Daten an ein eigenes Netzwerk:*__ Daten &auml;ndern in Zeile   
`MqttClientKH client("..ssid..", "..password..","mqtt server name");`   

## MQTT Nachrichten   
Um das System zu testen sind folgende Schritte erforderlich:   
* Verbinden mit dem WLAN (siehe oben)   
* &Ouml;ffnen eines Kommando-Fensters   
(MS Windows cmd oder eine putty-Verbindung zum Raspberry Pi mit dem User `pi`, Passwort `raspberry` oder `..ras..` oder einem anderen)   
* Anzeigen aller relay1-Nachrichten durch Eingabe von `mosquitto_sub -h 10.1.1.1 -t relay1/# -v`    
* &Ouml;ffnen eines zweiten Kommando-Fensters   
* Senden eines der unten stehenden Kommandos zB   
`mosquitto_pub -h 10.1.1.1 -t relay1/get -m help`   
* Im ersten Fenster sollte folgende Anzeige erscheinen: 
```
pi@raspberrypi:~ $ mosquitto_sub -h 10.1.1.1 -t relay1/# -v
relay1/get help
relay1/ret/help get: help|version|function|ip|all|lamp|current|current0|
set: lamp|current0|
```


### _set_ Kommandos   
* Topic `relay1/set/lamp` mit payload `on`, `off` oder `toggle` schaltet das Relais ein, aus oder um.   
* Topic `relay1/set/current0` setzt das "Strom Null" Limit (Wert in mA). Stromwerte von 0 bis zu diesem Wert ergeben "Kein Strom" (lamp=0). Dies ist wegen der kleinen Wandler-Spannungen und den damit verbundenen Einstreuungen sinnvoll.   

### _get_ Anfragen   
* Topic `relay1/get` mit payload `help` listet alle MQTT-Befehle auf, die das System versteht.   
* Topic `relay1/get` mit payload `version` ergibt das Software-Datum und den Dateinamen.   
* Topic `relay1/get` mit payload `function` ergibt eine kurze Beschreibung, was das System macht.   
* Topic `relay1/get` mit payload `ip` ergibt die IP-Adresse des D1 mini.   
* Topic `relay1/get` mit payload `all` bewirkt ein Aussenden aller m&ouml;glichen GET-Nachrichten.    
* Topic `relay1/get` mit payload `lamp` ergibt den Zustand der Lampe als 0 oder 1 (Lampe ist aus oder ein).   
* Topic `relay1/get` mit payload `current` ergibt den Stromwert in mA.   
* Topic `relay1/get` mit payload `current0` ergibt das Stromlimit f&uuml;r die Anzeige "Null" in mA.  

## Hardware
(1) WeMos D1 mini   
(2) Relais Shield   
(3) D1_INA122_V2 180924 (Selbstbau) oder Analog in mit Stromwandler ASM-010   
![D1 INA122 relay](./images/D1_ina122_relay.png "D1mini mit selfmade INA122 shield and relay shield")   
_D1mini mit Eigenbau INA122 Shield, Relais Shield, Stromwandler ASM-010 und Lampe_

## Software
Dieser Sketch verwendet die Klassen      
* `D1_class_MqttClientKH` und `PubSubClient`    
* `D1_class_Relais1` und `D1_class_Ain`    
* `D1_class_Statemachine`   

(Siehe Verzeichnis `src`)   

