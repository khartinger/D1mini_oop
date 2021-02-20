# D1 mini: 230V voltage monitoring with MQTT message
Sketch: D1_oop87_smqtt_voltagemonitoring1.ino, Version 2021-02-20   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   

After switching on the power supply, the D1 mini sends the MQTT message `voltage/1/ret/status` with payload `1`. If the supply voltage is switched off, it sends the same message with payload `0`.   
In DEBUG mode, the time until the message is sent and until the same message arrives back at the D1mini is also measured (usually 52ms or 104ms). The values are displayed via the serial interface (115200Bd).   

**Example for the MQTT messages**   
The supply voltage is switched on and switched off after some time.   
```
info/start voltage/1
voltage/1/ret/status 1
voltage/1/ret/status 0
```

## Hardware
1. WeMos D1 Mini or ESP32 D1 mini
2. 5V power supply with voltage divider, diode and backup capacitor

![Voltage monitoring](./images/D1_voltagemonitoring1.png "Voltage monitoring")   
_Figure 1: 230V/5V power supply unit and D1 mini with voltage monitoring shield_   

## Voltage Monitoring Shield
### Circuit
![Circuit of the voltage monitoring](./images/D1_voltagemonitoring1_circuit.png "Circuit of the voltage monitoring")   
_Figure 2: Circuit of the voltage monitoring shield_   

### Dimensioning
#### Resistors
If the supply voltage falls below 4.5V, D5=0 should be set. This happens when the input voltage is less than 1.6V:   
Uin = 4.5V, UR2 = 1.6V -> UR1 = Uin - UR2 = 2.9V.   
This gives for the ratio of the resistors   
R1:R2 = 2.9:1.6 = 1.81:1   
__Selected: R2 = 10k&Omega;, R1 = 18k&Omega;__   

Resistor R3 is used to limit the current when switching on (charging current for C1):   
__Selected: Resistor R3 = 1 &Omega;__   

#### Cut-off for C1   
Assumptions:   
* The current consumption of the D1 mini is between 70mA and 120mA.   
* The time from detection of the voltage drop to the end of sending the MQTT message is approx. 52ms, or until the message arrives back at the D1 mini is 104ms.   
* The capacitor is charged to approx. 4.7V.   

A rough estimate can be made of the electric charge needed to keep the D1 mini running (use static values instead of the real time courses):   
Q = C * U = I * t   
C = I * t / U = 120mA * 104ms / 4.7V = 0.12 * 0.104 / 4.7 = 2.7 mF = 2700&micro;F   
Tests show that a 3300&micro;F capacitor is sufficient. To be on the safe side, a gold-cap __capacitor 0.22F/5.5V__ is chosen.   
