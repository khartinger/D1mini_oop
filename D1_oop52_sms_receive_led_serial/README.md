# D1 mini: Switch a blue LED by SMS
Version 2018-12-30, File: `D1_oop52_sms_receive_led_serial.ino`   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

D1 mini waits for a SMS with content `#led on#` or `#led off#` and turns the blue LED on or off.   
* SIM-Connection   : Software Serial @ D5 (=TX), D6 (=RX)   
* Status info to PC: Hardware Serial @ TX, RX   

* Class __`GSMmodule`__: Choose connection while creating the object. No parameter uses software serial, a parameter `false` chooses hardware serial. E.g.      
`GSMmodule mod1;                        // SIM module soft serial`   
`//GSMmodule mod1(false);               // false=Hardware serial`   

## Hardware
1. WeMos D1 mini   
2. Battery Shield for WEMOS D1 mini with a little rechargeable battery HM04S (3.7V 350mAh 1.3Wh)   
3. Selfmade SIM800C Adapter for "SIM800C GSM GPRS module STM32" (ProtoBoard Shield)   

![D1 SIM800C_oled1](./images/D1_SIM800C_oled1.png "D1mini with SIM800C 9pol ProtoBoard Shield")   
_Figure 1: D1mini with Battery Shield, 9pol ProtoBoard Shield and 2-Button OLED Shield._ 

## Software - Details
On program start LED output is initialized and D1 mini tries to do settings by method `begin()`: Turn echo off, set SMS text format, use modem memory (ME), calculate number of first SMS in memory and check, to which network the SIM connected.   
Then the program deletes SMS location #1 and waits for a SMS in `loop()`.   
If a SMS comes in, the text is analysed, the command is executed, result is shown on serial and SMS location #1 is deleted.   

__*Example for output on Serial*__
```
2018-12-30 D1_oop52_sms_receive_led_serial
Wait for SMS #led on# or #led off#
-----Init Modem---
-----Modem OK-----
Delete location for command sms: #~OK#~
-----Ready--------
SMS received!
Number: 31 |
Phone : +43680xxxxxxx | len=13
Date  : 18/12/30,09:50:39+04 | len=20
Text  : #led on# | len=8
*** LED on  ***
SMS received!
Number: 31 |
Phone : +43680xxxxxxx | len=13
Date  : 18/12/30,09:51:20+04 | len=20
Text  : #led off# | len=9
*** LED off ***
```

## Special features of the hardware structure  
* The power supply of the SIM module is done by a battery shield and a rechargeable battery, because the D1 mini is not able to deliver sufficient (peak) energy.   
* SIM module is connected by a selfmade SIM800C adapter and software serial (D5=TX, D6=RX)   
* Optional: PC is connected by hardware serial TX/RX-pins and a 3.3V-RS232-Adapter.   
While testing you also can use the Arduino-IDE and USB-connection.   

![D1_SIM800_shield_circuit](./images/D1_SIM800_shield_circuit1.png "D1mini with D1_SIM800 shield - circuit")   
_Figure 2: Circuit of D1 mini SIM800 shield._ 

## Prepare the SIM module
* Buy a micro SIM card, register it and top up your credit.   
  For Austria: Use a Pre-Paid-SIM e.g. from Drei, HOT or B.free.   
* Insert micro SIM card into SIM module and connect to a PC via 3.3V-RS232-Adapter   
* The red LED of the SIM module indicates the state:   
--> Blinking at short intervals indicates, that the module is booting or trying to log in to a provider network.   
--> A flashing LED every 3 seconds indicates, that the module is ready to send or receive SMS.   
* If the LED begins to blink in short intervals while sending a SMS, this indicates, that the module boots again. Perhaps  the voltage supply is too weak.   



## Control the SIM module
The software control of the SIM module is done by AT commands.   
| Command | Meaning |   
|---------|---------|   
| `ATE0`  | Force no sending back commands (no echo). Answer should be "OK". |    
| `ATE1`  | Force sending back commands (echo). Answer should be "OK". |    
|`AT+CMGF=1` | Set text format. |    
|`AT+CPMS="ME","ME","ME"` | Define memory location for SMS (ME=Modul). |    
|`AT+CNMI=2,1,2,1,0` | Info, when a SMS is received. |    
| `AT+CMGS="0680xxxxxxx"` | Send a SMS. After sending the command a greater than sign &gt; appears and module waits for the SMS text. <br>__Important__: Finish SMS text with &lt;Strg&gt;Z = 0x1A |   
|`AT+CMGR=21` | Read SMS at location 21. Result might be:<br>`AT+CMGR=21<cr>`<br>`+CMGR: "REC UNREAD","+43680xxxxxxx","","18/12/15,17:06:50+04"<cr><lf>`<br>`#led on#<cr><lf><cr><lf>OK<cr><lf>` |   

### Some more helpful AT commands
| Command | Meaning |   
|---------|---------|   
| `AT+CPIN?`  | Check, if SIM card is present and unlocked. Answer should be `+CPIN: READY` |    
| `AT+CGREG?` | Check the registration status of the device. Answer should be `+CGREG: 0,2` if SIM card is registred in the home network. |    
| `AT+COPS?`  | Get the network information. Answer should be `+COPS: 0,0,"one"` if the device is registered on the home network (or "A1", "T-Mobile A" etc.) |    
| `AT+CSQ`    | Get the signal quality. Answer may be `+CSQ: 6,0`. <br>Values for signal strength: 0 = 113dBm or less (no signal), 2..9 marginal, 10..14 OK, 15..19 good, 20..30 excellent, 31 = 51dBm or greater (better), 99 unknown. |    
| `ATD08006642903;` | Dial the number for a voice call (e.g. ask for a sms with credit content when using A1 prepaid SIM). <br>Don't forget to hang up with `ATH`! |    
| `ATH`       | Hangup a voice call. |    
| `ATS0=1`    | Set number of "RING"s before picking up an incomming call automatically. Don't forget to hang up with `ATH`! Disable auto-answering feature with `ATS0=0`. |    
| `ATA`       | Pick up an incoming voice call. Don't forget to hang up with `ATH`! |    
| `AT+CLCC`   | List Current Calls: Check, if a voice call is in progress. |    
| `AT+COPS=0` | Force network registration. |    
| `AT+COPS=1,0,"A1"` | Manual network registration to network "A1". |    
| `AT+CCID`   | Number of SIM card, e.g. 8943102101xxxxxxxxxf |    
| `AT+CNUM`   | Should show the phone number of the device (not implemented). |    
