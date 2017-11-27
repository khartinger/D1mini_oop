# D1 mini: Beispiele zur Verwendung der Klasse DS1307 (Real Time Clock)
Sketch: -, Version 2017-11-27   
[English Version](./README.md "English Version")

Dieses Verzeichnis enth&auml;lt Beispielprogramme, die die Verwendung der Klasse **DS1307** zur Kommunikation mit der Echtzeituhr (Real Time Clock, RTC) DS1307 &uuml;ber den I2C-Bus zeigen.
 Die Klasse dient nur zum Datenaustausch zwischen einem Programm und der DS1307 Uhr und ber&uuml;cksichtigt nicht die Sommerzeit, etc.     

**Info**   
* D1_oop23_DS1307_demo: Nutze verschiedene Methoden zum Schreiben und Lesen von Datum und Uhrzeit sowie zu RAM-Operationen. Zeige die Ergebnisse &uuml;ber die serielle Schnittstelle an.
* D1_oop23_DS1307_setSerial: Setze Datum und Uhrzeit der RTC DS1307 &uuml;ber die serielle Schnittstelle und zeige die Werte regelm&auml;&szlig;ig &uuml;ber die serielle Schnittstelle an.
* D1_oop23_DS1307_setTimeHttp: &Uuml;ber die serielle Schnittstelle kann Datum und Uhrzeit entweder auf einen Vorgabewert gesetzt (Befehl r) oder von einem Server (zB Gateway) geholt (Befehl s) werden. Die Werte werden regelm&auml;&szlig;ig &uuml;ber die serielle Schnittstelle angezeigt.

## Hardware
* WeMos D1 mini
* Datenlogger Shield (microSD + DS1307RTC Shield)   
  DS1307  verwendet I2C (D1-SCL, D2-SDA)   
  microSD verwendet SPI (D5-CLK, D6-MISO, D7-MOSI, D8-CS)   

![Bild: D1mini microSD + DS1307 shield](./images/D1_microSD_DS1307_shield.png "D1mini microSD + DS1307 shield")


## Klassendiagramm DS1307
| Klasse DS1307                     |     |
| --------------------------------- | --- |
| + __DS1307__()                    | Konstruktor: Setzen der Vorgabewerte f&uuml;r I2C-Adresse, setup()   |
| + DS1307(int i2c_address)         | Konstruktor: Setzen der gegebenen I2C-Adresse, setup() |
| + int    getAddress()             | Hole I2C-Adresse (Vorgabe ist (x68)  |
| + int    getStatus()              | Hole Status: <br>0: Erfolg, 1: Daten zu lang f&uuml;r den Sendepuffer, 2: Empfang von NACK beim Senden der Adresse, <br>3: Empfang von NACK beim Senden der Daten, 4: Anderer Fehler, 5: I2C falsche Anzahl Bytes<br>6: Kein Byte empfangen, 7: Falsche Anzahl Bytes, 8: Falscher Wertebereich, 9: Fehler beim Lesen des RAM |

&nbsp;

|     *Datum/Uhrzeit Methoden*                  |     |
| --------------------------------------------- | --- |
| + int setDate(String yyyymmdd)                | Schreibe das Datum in die DS1307-Uhr   |
| + int setDate(int yyyy, byte mm, byte dd)     | Schreibe das Datum in die DS1307-Uhr     |
| + int setTime(String HHMMSS)                  | Schreibe die Uhrzeit in die DS1307-Uhr |
| + int setTime(byte HH, byte MM, byte SS)      | Schreibe die Uhrzeit in die DS1307-Uhr   |
| + int __setDateTime(String yyyymmdd_HHMMSS)__ | Schreibe Datum und Uhrzeit in den DS1307 |
| + int setSecs1970(unsigned long secs1970)     | Wandle die Sekunden seit 1.1.1970 in Datum und Zeit um und schreibe sie in den DS1307 |
| + unsigned long getSecs1970()     | Lies Datum und Zeit vom DS1307 und wandle sie in Sekunden seit 1.1.1970 um |
| + String getDate()                | Hole Datum, wandle es um ins Format yyyymmdd    |
| + String getDate2()               | Hole Datum, wandle es um ins Format dd.mm.yyyy  |
| + String getTime()                | Hole Uhrzeit, wandle sie um ins Format HHMMSS   |
| + String getTime2()               | Hole Uhrzeit, wandle sie um ins Format HH:MM:SS |
| + String __getDateTime()__        | Hole Datum und Uhrzeit, wandle sie um in yyyymmdd HHMMSS |
| + String __getDateTime2()__       | Hole Datum und Uhrzeit, wandle sie um in dd.mm.yyyy HH:MM:SS |
| + String getWeekday()             | Liefert deutsche Abk&uuml;rzungen f&uuml;r die Wochentage (SO, MO, DI, MI, DO, FR, SA or ??) |

&nbsp;

| * zuletzt eingelesene Werte f&uuml;r Datum und Uhrzeit (ohne DS1307 Zugriff)        |     |
| ----------------------------------------------------------------------------------- | --- |
| + String getLastDate()            | Letztes Datum, Format yyyymmdd, kein DS1307 Zugriff   |
| + String getLastDate2()           | Letztes Datum, Format dd.mm.yyyy, kein DS1307 Zugriff |
| + String getLastTime()            | Letzte Uhrzeit, Format HHMMSS, kein DS1307 Zugriff    |
| + String getLastTime2()           | Letzte Uhrzeit, Format HH:MM:SS, kein DS1307 Zugriff  |
| + String getLastDateTime()        | Letztes Datum+Uhrzeit, Format yyyymmdd HHMMSS, kein DS1307 Zugriff |
| + String getLastDateTime2()       | Letztes Datum+Uhrzeit, Format dd.mm.yyyy HH:MM:SS, kein DS1307 Zugriff |
| + String getLastWeekday()         | Letzte deutsche Abk&uuml;rzung f&uuml;r die Wochentage (SO, MO, DI, MI, DO, FR, SA or ??, kein DS1307 Zugriff  |
| + unsigned long getLastSecs1970() | Letzter Wert an Sekunden seit 1.1.1970 |

&nbsp;
  
| *Verwendung des DS1307 RAM*                 | *(Adresse 0x08...0x3F)* |
| --------------------------------------------| ----------------------- |
| + int clearRAM()                            | setzt alle RAM-Bytes auf 0, verwendet clearRAM(initbyte) |
| + int    clearRAM(byte initbyte)            | Schreibt das RAM mit initbyte voll und kontrolliert, ob alle Bytes den richtigen Wert haben (R&uuml;ckgabe status) |
| + int    setRAMbyte(int address, byte data) | Schreibe Datenbyte ins DS1307 RAM         |
| + byte   getRAMbyte(int address)            | Lies Datenbyte aus dem DS1307 RAM         |
| + int  setRAMint16(int address, int int16)  | Schreibe ganze Zahl (int) ins DS1307 RAM  |
| + int  getRAMint16(int address)             | Lies ganze Zahl (int) aus dem DS1307 RAM  |

&nbsp;

| *&Ouml;ffentliche Hilfsmethoden*                |                                      |
| ----------------------------------------------- | ------------------------------------ |
| + String date2string(int yyyy, int mm, int dd)  | Wandle Datum um in String yyyymmdd   |
| + String date2string2(int yyyy, int mm, int dd) | Wandle Datum um in String dd.mm.yyyy |
| + String time2string(int hh, int mm, int ss)    | Wandle Uhrzeit um in String HHMMSS   |
| + String time2string2(int hh, int mm, int ss)   | Wandle Uhrzeit um in String HH:MM:SS |
| + int    isLeapyear(int year)                   | Ist das Jahr (4 Stellen) ein Schaltjahr? |
| + bool  isSummertime(int yyyy, byte mm, byte dd, byte HH, byte timezone)| true: Datum/Uhrzeit liegt in der Sommerzeit (DST) | 
| + bool   __isSummertime(byte timezone)__        | true: gespeicherte Datum/Uhrzeit liegt in der Sommerzeit (DST) |

&nbsp;

|  *Protected Hilfsmethoden*                    |                                        |
| --------------------------------------------- | -------------------------------------- |
| ~ void setup()                                | Start des Takts, 24-Stunden-Mode, initialisieren der Eigenschaften |
| ~ int  bcd2byte(byte bcd)                     | Umwandlung BCD-Byte in Integer-Wert    |
| ~ byte byte2bcd(int value8bit)                | Umwandlung 8 bit Wert (00..99) in BCD  |
| ~ byte getDayOfWeek()                         | Berechne den Wochentag f&uuml;r das aktuelle Objekt 0=SO, 1=MO, 2=DI, 3=MI, 4=DO, 5=FR, 6=SA |
| - byte getDayOfWeek(int yyyy, int mm, int dd) | Berechne den Wochentag f&uuml;r die gegebenen Werte |
| ===== *Datenkontrolle* =====                  |                                           |
| ~ bool isDateOK(int yyyy,int mm,int dd)       | Pr&uuml;fe, ob das Datum richtig ist      |
| ~ bool isDateOK(String yyyymmdd)              | Pr&uuml;fe, ob das Datum richtig ist      |
| ~ bool isTimeOK(int HH, int MM, int SS)       | Pr&uuml;fe, ob die Zeitangabe richtig ist |
| ~ bool isTimeOK(String HHMMSS)                | Pr&uuml;fe, ob die Zeitangabe richtig ist |
| ===== *I2C Zugriff* =====                     |                                           |
| - int  write1reg(byte reg, byte value)        | Schreibe Byte an RAM-Adresse reg (return: status) |
| - byte read1reg(byte reg)                     | Lies ein Byte von der RAM-Adresse reg  |
| - int  setDateRegs(String yyyymmdd)           | Kopiere Datum in die DS1307-Register   |
| - int  setTimeRegs(String HHMMSS)             | Kopiere Uhrzeit in die DS1307-Register |
| ~ String readDate()                           | Kopiere Datum in die DS1307-Register, R6uuml;ckgabe yyyymmdd |
| ~ String readTime()                           | Kopiere Uhrzeit in die DS1307-Register, R6uuml;ckgabe HHMMSS |

| --- *properties* ---        |     |
| --------------------------- | --- |
| - int  i2cAddress;          | verwendete i2c-Adresse (Vorgabe 0x68) |
| - int  status;              | 0=ok, >0 Fehler |
| - int  yy_;                 | Jahr (00..99) |
| - int  mm_, dd_;            | Monat (1..12), Tag (1..31)  |
| - int  HH_, MM_, SS_;       | Stunde (0..23), Minute (0..59), Sekunde (0..59) |
| - int  ww_;                 | Nummer des Wochentages: 0=Sonntag, 1=Montag, .. |
