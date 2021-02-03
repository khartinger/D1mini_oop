//_____D1_oop34_BME280.ino____________________180326-210202_____
// Measure temperature, humidity pressure/altitude with a BME280
// and send values to Serial
// Hardware: WeMos D1 Mini
//           BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x76)
// Updates:
// 210202: add chipId, chipAddress, getSensorName
// Released into the public domain.

#include "./src/D1_class_BME280.h"
BME280 bme280;                              // sensor object

//_____SETUP____________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //bme280.setParams();
 //------search for sensor--------------------------------------
 bme280.setAddress(0x76);                   // 0x76 or 0x77
 bool status = bme280.begin();              // start I2C
 if (!status) {
  bme280.setAddress(0x77);                  // 0x76 or 0x77
  status = bme280.begin(false);             // NO start I2C
  if (!status) {
   Serial.println("Error: "+bme280.getsStatus());
   Serial.println("Could not find BME280 sensor:");
   Serial.println("* check wiring");
   Serial.println("* check whether Wire.begin() has been called");
   Serial.println("* check I2C address (0x76 or 0x77)\nEND!");
   while (1) delay(100);
  }
 }
 //------info about the sensor chip, wait a little bit----------
 Serial.println(bme280.getSensorName()+" found!");
 Serial.printf("I2C address: 0x%02X\n",bme280.getAddress());
 Serial.println("Wait until the first measure has been completed...");
 while(!bme280.measuring()) { delay(100); }
 Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 float t,h,p,a;
 //-----get all values at once----------------------------------
 //bme280.getValues(t,h,p,a);
 //-----get values as string------------------------------------
 Serial.println(bme280.getsJson());
 Serial.println(bme280.getsValues(" | ",2,2,0,1));
 //-----get values one by one-----------------------------------
 t=bme280.getTemperature();
 h=bme280.getHumidity();
 p=bme280.getPressure();
 a=bme280.getAltitude();
 Serial.print("T=");Serial.print(t);Serial.print("*C, ");
 Serial.print("H=");Serial.print(h);Serial.print("%, ");
 Serial.print("p=");Serial.print(p);Serial.print("hPa, ");
 Serial.print("a=");Serial.print(a);Serial.println("m");
 Serial.println("--------------------------------------------");
 delay(2000);
}
