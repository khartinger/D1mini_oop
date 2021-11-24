# Distance measurement with LIDAR TFmini and display on OLED 0.66"
Sketch: D1_oop98_tf02pro_oled66_1.ino, Version 2021-11-24   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   

## Function
This program for the D1 mini or ESP32mini (ESP32-WROOM-32) does the following:   
1. periodically read TFmini data via serial interface and calculate the distance (in cm).   
2. display of the distance on a display (OLED 0,66").   
3. output of a melody, if a minimum distance (limit) is fallen below.   
4. setting of the limit value by means of a potentiometer. (Read in as analog value)   

## Hardware required
1. D1 mini or ESP32mini (ESP32-WROOM-32)   
2. do-it-yourself TF adapter shield: potentiometer for input of analog value, pins for LIDAR connection (TFmini or TF02)   
3. buzzer shield @ D5 (GPIO18)
4. OLED 0.66" shield @ I2C
5. LIDAR TFmini (0.1m to 12m) or TF02 (0.1m to 40m)
