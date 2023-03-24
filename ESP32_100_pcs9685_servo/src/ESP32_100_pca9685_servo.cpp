//_______ESP32_100_pca9685_servo.cpp_____________khartinger_____
// Template: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/blob/master/examples/servo/servo.ino
// This is an example for a D1mini or ESP32-D1mini with an 
// Adafruit 16-channel PWM & Servo driver and a servo at output 0.
// The servo moves back and forth between the minimum and maximum
// positions and waits 2 seconds in each of the end positions.
// Hardware:
// (1) ESP32 D1 mini or WeMos D1 mini
// (2) PCA9685 board e.g. http://www.adafruit.com/products/815
// (3) Ultra Micro 1,5 g Linear Servo or SG90 9G Micro Servo Motor
// Connect:
// * D1mini  - PCA9685 board (I2C bus)
//   (1) D1  - SCL
//   (2) D2  - SDA
//   (3) GND - GND
//   (4) 3V3 - VCC
// * PCA9685 Board: power supply servo: V+ - 5V, GND - GND
// Created by Karl Hartinger, March 23, 2023.
// Changes:
// 2023-03-23 New
// Released into the public domain.

#include <Wire.h>
#include <Adafruit_PWMServoDriver/Adafruit_PWMServoDriver.h>

//_____sensors, actors, global variables________________________
// ------Use default i2c address 0x40---------------------------
Adafruit_PWMServoDriver servo0 = Adafruit_PWMServoDriver();

// ------Adjust these values depending on the servo used!-------
// #define SERVOMIN  280
// #define SERVOMAX  380
#define SERVOMIN  180 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  480 // This is the 'maximum' pulse length count (out of 4096)
// #define USMIN   900   // This is the 'minimum' microsecond length
// #define USMAX  2300   // This is the 'maximum' microsecond length
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define SERVO_NUM  0

//_______SETUP__________________________________________________
void setup() {
  Serial.begin(115200);
  Serial.println("Servo test!");
  servo0.begin();
  // servo0.setOscillatorFrequency(27000000); // default 25000000
  servo0.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
}

//_______LOOP___________________________________________________
void loop() {
  Serial.println("Pulses get longer...");
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
    servo0.setPWM(SERVO_NUM, 0, pulselen);
    delay(1);
  }
  delay(2000);
  Serial.println("Pulses get shorter...");
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
    servo0.setPWM(SERVO_NUM, 0, pulselen);
    delay(1);
  }
  delay(2000);
}

/*
//_______LOOP___________________________________________________
void loop() {
  Serial.println("Pulses get longer...");
  for (uint16_t microsec = USMIN; microsec <= USMAX; microsec++) {
    servo0.writeMicroseconds(SERVO_NUM, microsec);
  }
  delay(2000);
  Serial.println("Pulses get shorter...");
  for (uint16_t microsec = USMAX; microsec >= USMIN; microsec--) {
    servo0.writeMicroseconds(SERVO_NUM, microsec);
  }
  delay(2000);
}
*/