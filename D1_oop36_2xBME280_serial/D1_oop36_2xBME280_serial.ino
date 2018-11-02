//_____D1_oop36_2xBME280_serial.ino___________180926-181102_____
// Measure temperature, humidity and pressure/altitude with two
// BME280 every 2 seconds and print values on Serial (115200Bd)
// Hardware: WeMos D1 Mini
//           2xBME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x76 and 0x77)

#define I2C_ADDRESS2              0x77
#include "src/bme280/D1_class_BME280.h"
BME280 bme280_1;                            // sensor object 1
BME280 bme280_2(I2C_ADDRESS2);              // sensor object 2
String s1;

//_____print BME280 info to serial______________________________
void printBME280info(BME280 bme280x)
{
 int id=bme280x.getID();
 s1="Sensor type at address 0x"+String(bme280x.getAddress(),16)+": ";
 if(id==0x60) s1+="BME280";
 else {
  if(id==0x58) s1+="BMP280 (no humidity value!)";
  else s1+="unknown (sensor missing?)";
 }
 Serial.println(s1);
 Serial.println("  Status: "+bme280x.getsStatus());
}

//_____setup____________________________________________________
void setup() {
 Serial.begin(115200); Serial.println();
 bme280_1.begin();  
 printBME280info(bme280_1);
 bme280_2.begin();  
 printBME280info(bme280_2);
}

//_____loop_____________________________________________________
void loop() {
 //-----Sensor 1------------------------------------------------
 s1="Sensor 0x"+String(bme280_1.getAddress(),16)+": ";
 s1+=bme280_1.getsJson();
 Serial.println(s1);
 //-----Sensor 2------------------------------------------------
 s1="Sensor 0x"+String(bme280_2.getAddress(),16)+": ";
 s1+=bme280_2.getsJson();
 Serial.println(s1);
 Serial.println("--------------------------------------------");
 delay(2000);
}
