//_____D1_oop34_BME280.ino____________________180326-180326_____
// Measure temperature, humidity pressure/altitude with a BME280
// and send values to Serial
// Hardware: WeMos D1 Mini
//           BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x76)

#include "D1_class_BME280.h"
BME280 bme280;                         // sensor object

void setup() {
 Serial.begin(115200);                 // init serial comm
 Serial.println();                     // next line
 //bme280.setParams();
 bool status = bme280.begin();  
 if (!status) {
  Serial.println("Could not find BME280 sensor, check wiring!");
  while (1);
 }
 Serial.println("BME280 found!");      // start message
}

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

