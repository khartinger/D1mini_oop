# D1 mini: Schalten der blauen LED eines anderen D1 mini &uuml;ber MQTT
Version 2020-03-08, Sketch: D1_oop72_mqtt2_button2blueled.ino   
[English version](./README.md "English version")  

Der Taster dieses D1 mini steuert die blaue LED eines anderen D1 mini, auf dem zB das Programm `D1_oop71_mqtt2_blueled` l&auml;uft.   
* Wird der Taster gedr&uuml;ckt, sendet der D1 mini eine Nachricht mit Topic `oop71/set/blueled` und Payload `2`.   
* Empf&auml;ngt der D1 mini eine Nachricht mit Topic `oop72/set/blueled` und Payload `0`, `1` oder `2`, so schaltet er seine eigene LED aus, ein oder um.   
* Gibt es keine Verbindung zum MQTT-Server, blinkt die blaue LED schnell.   

Um zu zeigen, dass das Programm (noch) l&auml;uft, wird die LED nicht nur aus- und eingeschaltet, sondern sie blinkt mit einer Periodendauer von 2 Sekunden:   
* "aus" = 1.9s LED aus, 0.1s LED ein (= kurze Lichtimpulse)
* "ein" = 0.1s LED aus, 1.9s LED ein  (= kurze "Aussetzer")

### Struktur der MQTT Nachrichten
* Anfrage `oop71/get` _help_         : liefert alle sGet-, sSet,- sSub- und sPub-Topics als Antwort
* Anfrage `oop71/get` _version_      : fragt nach der Version des Programmes (zB 2020-03-08 D1_oop71_mqtt2_blueled)   
* Anfrage `oop71/get` _ip_           : liefert die IP des D1minis als Antwort   
* Anfrage `oop71/get` _blueled_      : liefert den Zustand der blauen LED als Antwort (0 oder 1)   
* Befehl  `oop71/set/blueled` _0|1|2_: setzt die blaue LED auf aus, ein oder toggle   
* Antwort  `oop71/ret/blueled` _0|1_ : Antwort auf die get-Anfrage   

Zur einfacheren Programmierung wird innerhalb der Hauptschleife loop() eine Zustandsmaschine (Klasse `Statemachine`) und die Klasse `StatemachineBlink` zur Ansteuerung der LED verwendet.   
Statemachine: Zykluszeit 1 Minute (600 states mit je 0,1 Sekunden)   

__*Wichtig*__: Das Programm ben&ouml;tigt einen MQTT-Broker!   
Die WLAN- und MQTT-Server-Daten an das eigene Netzwerk anpassen in der Zeile:   
`MqttClientKH client("..ssid..", "..password..","mqtt server name");`  

## Hardware 
1. WeMos D1 mini

## Software
Das Programm verwendet vier Klassen:   
* `Statemachine`: Schrittz&auml;hler f&uuml;r die zeitliche Steuerung
* `StatemachineBlink`: Steuerung des Blinkens einer LED (ben&ouml;tigt Klasse `Statemachine`)
* `MqttClientKH2`: Diese Klasse erweitert die Klasse `PubSubClient` zur einfacheren Verwendung von MQTT. Es k&ouml;nnen alle Befehle der Klasse `PubSubClient` verwendet werden.
* `Din`: Mit Hilfe dieser Klasse wird das Dr&uuml;cken des Tasters festgestellt (fallende Flanke).

__*Anmerkung*__: Ist die Bibliothek `PubSubClient` in der Arduino-IDE installiert, k&ouml;nnen die Dateien `PubSubClient.*` im Verzeichnis `/src/mqtt` gel&ouml;scht werden.

### Ideen zu diesem Programm
* __*Bezeichnung von Topics und Payloads*__: Um das Programm als Vorlage f&uuml;r andere Programme verwenden zu k&ouml;nnen, sind alle MQTT-Bezeichnungen in Feldern gespeichert oder als Konstante definiert. Beispiele:   
`sGet[]` enth&auml;lt alle Payloads f&uuml;r Anfragen mit dem Topic ".../get", z.B.   
`sGet[GETMAX72]={"help","version","ip","blueled"};`   
`sSet[]` enth&auml;lt den letzten Teil der Topics ".../set/...", mit denen Werte gesetzt werden k&ouml;nnen, z.B.   
`sSet[SETMAX72]={"blueled"};`   
`sSub[]` enth&auml;lt "fremde" Topics, auf die der D1 mini h&ouml;rt (**sub**scribe) z.B.   
`sSub[SUBMAX72]={"oop71/ret/blueled"};`   
`sPub[]` enth&auml;lt "fremde" Topics, die der D1 mini sendet (**pub**lish) z.B.   
`sPub[PUBMAX72]={"oop71/set/blueled"};`   

* `callback()`: Diese Funktion wird von allen registrierten (subscribe), eintreffenden MQTT-Nachrichten aufgerufen. Um die Ausf&uuml;hrungszeit kurz zu halten, werden __nur Bits__ in den Variablen ("Container") `iGet`, `iSet` oder  `iSub` __gesetzt__. Payloads, zB f&uuml;r iSet oder iPub, werden in Feldern wie `sSetPayload[]` oder `sSubPayload[]` abgelegt.
* `sendRet()`: Diese Funktion sendet alle MQTT-Antworten f&uuml;r Anfragen, die das entsprechende Bit in  `iRet` oder `iPub` (zB durch `doGet()`) gesetzt haben.
* `doGet()`: Diese Funktion bearbeitet alle Get-Anfragen (dh, deren Bit in iGet gesetzt ist) und muss regelm&auml;&szlig;ig in der Hauptschleife loop() aufgerufen werden, da sonst keine Get-Aktionen ausgef&uuml;hrt werden. F&uuml;r jede Anfrage wird die Antwort vorbereitet, als Payload in `sRetPayload[]` gespeichert und das entsprechende Bit in `iRet` gesetzt.
* `doSet()`: Diese Funktion ist &auml;hnlich zu 'doGet()', allerdings werden auch Aktionen wie die &Auml;nderung von Einstellungen oder Hardware-Zugriffe durchgef&uuml;hrt, wobei die erforderlichen Parameter in `sSetPayload[]` stehen.   
F&uuml;r jede MQTT-Anfrage wird die entsprechende Aktion ausgef&uuml;hrt und &uuml;blicherweise auch eine Antwort vorbereitet. Diese wird als Payload in `sRetPayload[]` abgelegt und das dazugeh&ouml;rige Bit im "Container" `iRet` gesetzt.

Die loop() Funtion besteht aus sieben Teilen:
```
void loop() {
 //======SECTION 1: action at the beginning of loop=============
 int state=stm.loopBegin();            // state begin 
 // ...
 //======SECTION 2: check for WiFi and MQTT connection==========
 if(client.isWiFiConnected())
 {
  if(client.isConnected()) //MUST always be called (for receive!)
  {//====SECTION 3: action for WiFi=YES, MQTT=YES===============
   // ...
  }//====END OF SECTION 3=======================================
  else
  {//====SECTION 4: action for WiFi=YES, MQTT=NO================
   // ...
  }//====END OF SECTION 4======================================= 
 }//=====END OF SECTION 2: End of if=true=======================
 else
 {//=====SECTION 5: action for WiFi=NO (-> MQTT=NO)=============
  // ...
  //=====END OF SECTION 5=======================================
 }//=====END OF SECTION 2: End of else==========================
 //======SECTION 6: action for WiFi=DONT_CARE, MQTT=DONT_CARE===
 if(iGet>0) doGet();
 if(iSet>0) doSet(); 
 // ...
 //======SECTION 7: prepare for next state and wait=============
 led1.doBlink(stm);                    // blue led action
 stm.loopEnd();                        // state end
}
```

__*Zusammenfassung*__   
Der vollst&auml;ndige Ablauf zum Setzen der blauen LED und zum Erstellen einer Antwort sieht folgenderma&szlig;en aus:   
1. Ein Dr&uuml;cken des Tasters setzt das Bit 0 des "Containers" `iPub` und speichert den Wert `2` (= umschalten) in `sPubPayload[0]`.   
2. Ist der MQTT-Server erreichbar, wird in `loop()` Sektion 3 `sendRet()` aufgerufen, da iPub > 0 ist.   
3. In `sendRet()` wird die Nachricht zum Umschalten der anderen LED durch Topic sPub[0]=`oop71/set/blueled` mit Payload `2` (= Wert von sPubPayload[0]) ver&ouml;ffentlicht (published).   
------   
_Falls eine Antwort hereinkommt:_   

4. Die Funktion `callback()` stellt in Sektion 4 fest, dass eine "fremde" Nachricht mit dem Topic sSub[0]=`oop71/ret/blueled` eingetroffen ist, setzt Bit 0 in `iSet` und speichert die Payload in `sSetPayload[0]`.   
5. In `loop()` Sektion 6 ist der Wert von `iSet` gr&ouml;&szlig;er als 0, daher wird `doSet()` aufgerufen.   
6. In `doSet()` wird die LED umgeschaltet (da Bit i=0 ist) und eine Antwort ausgel&ouml;st:   
 `iRet|=(1<<3);` setzt das Bit 3 in iRet, um die gleiche MQTT-Nachricht wie f&uuml;r sGet[3]="blueled" mit Payload `sRetPayload[3]=String(iBlueLed);` auszul&ouml;sen.   
7. In `loop()` Sektion 3 ist der Wert von `iRet` gr&ouml;&szlig;er als 0, daher wird `sendRet()` aufgerufen.   
8. In `sendRet()` wird eine Nachricht mit Topic TOPIC_72_RET + sGet[3] = `oop72/ret/blueled` und Payload "0" or "1" (= Wert von sRetPayload[3]) ver&ouml;ffentlicht (published).   

Mith&ouml;ren mit Mosquitto ergibt z.B.   
```   
mosquitto_sub -h 10.1.1.1 -t "#" -v
oop71/set/blueled 2
oop71/ret/blueled 1
oop72/ret/blueled 1
```   
