# D1 mini: Schalten der blauen LED mit MQTT
Version 2018-10-13   
[English version](./README.md "English version")   

Dieses sehr einfache Beispiel zeigt den D1 mini als MQTT subscriber.
Wenn der D1 mini ein Topic `led` mit der Nachricht (message) `on` empf&auml;ngt, schaltet er die blaue LED ein, mit der Nachricht `off` schaltet er die blaue LED aus.

__*Nicht vergessen: Die WLAN-Daten an das eigene Netzwerk anpassen*__ in der Zeile:   
`MqttClientKH client("..ssid..", "..password..","mqtt server name");`  

### Hardware
(1) WeMos D1 mini   

### Software
Verwendet `PubSubClient` und `D1_class_MqttClientKH` (siehe Verzeichnis `src\mqtt`)  
