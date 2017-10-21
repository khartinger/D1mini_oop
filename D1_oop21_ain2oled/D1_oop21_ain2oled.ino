//_____D1_oop21_ain2oled.ino__________________171008-171021_____
// Show analog input value every second on oled and Serial
// Hardware: WeMos D1 mini
//           OLED Shield: SSD1306, 64x48 pixel, I2C
//                        SCL GPIO5, SDA GPIO4, OLED_RESET GPIO0
// The required Adafruit-files were copied from the zip-files
//  * Adafruit_SSD1306-esp8266-64x48.zip and
//  * Adafruit-GFX-Library-1.1.5.zip
// to directory /libs. 
// When Adafruit libs are installed, delete directory /libs!
#include "D1_class_DisplayKH.h"
DisplayKH display_;               // display object
const int pinAin=A0;              // analog in pin
const float ain_x2=1012, ain_y2=3.277, ain_x1=16, ain_y1=0.0187;
const float ain_k=(ain_y2-ain_y1)/(ain_x2-ain_x1);
const float ain_d=(ain_x2*ain_y1-ain_x1*ain_y2)/(ain_x2-ain_x1);

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(9600); Serial.println("");
 //-----init screen---------------------------------------------
 display_.screen112(1,"Analog In", 'c');    // line 1 (inverted)
 display_.screen112(2,"?",'c');             // line 2 (big)
 display_.screen112(3,"?",'c');             // line 3 (volt)
 display_.screen112(4,"V171021 KH");        // line 4 (version)
}

//_____main loop________________________________________________
void loop() 
{
 int ain1023 = analogRead(pinAin); // analog in value (0..1023)
 //-----Convert Ain (0...1023) to voltage (0...3.2V) and String-
 float voltage=ain_k*ain1023 + ain_d;
 String sAin1023=String(ain1023);
 String sVoltage=String(voltage);
 String s1=sAin1023+" = "+sVoltage+" V";
 //-----print out the value to serial---------------------------
 Serial.println(s1);
 //-----display oled--------------------------------------------
 display_.screen112(2,sAin1023,'c');        // line 2 = value
 display_.screen112(3,sVoltage+" V",'c');   // line 3 = volt
 delay(1000);  
}
