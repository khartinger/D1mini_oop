//_____D1_oop32_INA226.ino____________________180216-180218_____
// Measure current, voltage and power with CJMCU-226 board
// and send value via I2C.
// Max. voltage 36V, Rshunt=0,1Ohm
// Hardware: WeMos D1 Mini
//           CJMCU-226 board connected to I2C of D1 mini:
//           SCL=D1, SDA=D2, 3V3, GND
//           VBUS: Bus voltage in 0..36V, Alert: Open-drain out
// See also GIT: https://github.com/jarzebski/Arduino-INA226
//
#include "D1_class_INA226.h"
INA226 ina;

void setup() 
{
 Serial.begin(115200);                 // init serial comm
 Serial.println();                     // next line
 ina.setFactorCurrent(0.972);          // board specific
}

void loop()
{
 Serial.print("Shunt current: ");
 Serial.print(ina.readShuntCurrent()*1000, 3);
 Serial.print(" mA (");
 Serial.print(ina.readShuntVoltage()*1000, 3);
 Serial.print(" mV), Bus voltage: ");
 Serial.print(ina.readBusVoltage(), 3);
 Serial.print(" V, power: ");
 Serial.print(ina.readBusPower(), 3);
 Serial.println(" W");
 delay(5000);
}
