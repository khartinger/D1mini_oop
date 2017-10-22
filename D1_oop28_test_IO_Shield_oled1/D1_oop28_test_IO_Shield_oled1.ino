//_____D1_oop28_test_IO_Shield_oled1.ino______171022-171022_____
// Function: Red LED lights up for 2s, green LED 0,5s, ...
//           If button is pressed, both LEDs are on
//            (=> yellow, if DUO-LED is used).
// Hardware: (1) WeMos D1 mini
//           (2) D1_IO_Shield1: 2x LEDs at D7 (green), D8 (red)
//                              1x Button at D5
//           (3) OLED Shield: SSD1306, 64x48 pixel, I2C
//               SCL GPIO5, SDA GPIO4, OLED_RESET GPIO0
// Software:
// The required Adafruit-files were copied from the zip-files
//  * Adafruit_SSD1306-esp8266-64x48.zip and
//  * Adafruit-GFX-Library-1.1.5.zip
// to directory /libs. 
// When Adafruit libs are installed, delete directory /libs!
#include "D1_class_DisplayKH.h"
#include "D1_class_Din.h"
#include "D1_class_Dout.h"
//-----for control program states-------------------------------
#define STATE_NO      -1          // nothing to do
#define STATE_0        0          // 0...1,9s
#define STATE_1       20          // 2,0...2,5s
#define STATE_2       25          // back to STATE_0
int state=STATE_0;                // state (=0,1 sec counter)
int state_restore=STATE_NO;       // state when btn pressed
//-----objects for io-access------------------------------------
DisplayKH display_;               // display object
Din   pin_button(D5);             // button (btn)
Dout  led_green(D7);              // green led @ D1_OnOff shield
Dout  led_red(D8);                // red led @ D1_OnOff shield

void setup() {
 //-----init screen---------------------------------------------
 display_.screen221(1,"HTL", 'c');     // line 1
 display_.screen221(2,"KAINDORF",'c'); // line 2
 display_.screen221(3,"?",'c');        // line 3 (LED green)
 display_.screen221(4,"?",'c');        // line 4 (LED red)
 display_.screen221(5,"171022 ht",'c');// line 5 (version)
}

void loop() 
{
 bool in1_;
 in1_= pin_button.isPressed();
 if(!in1_) //=====button not pressed============================
 { 
  if((state==STATE_0)||(state_restore==STATE_0))
  { //-----red LED on-------------------------------------------
   led_red.on();                            // led red
   led_green.off();                         // -
   display_.screen221(3,"",'c');            // oled green
   display_.screen221(4,"LED rot",'c');     // oled red
   state_restore=STATE_NO;
  }
  if((state==STATE_1)||(state_restore==STATE_1))
  { //-----green LED lights up----------------------------------
   led_red.off();                           // -
   led_green.on();                          // led green
   display_.screen221(3,"LED gr\x81n",'c'); // oled green
   display_.screen221(4,"",'c');            // oled red
   state_restore=STATE_NO;
  }
  state++;
  if(state>STATE_2) { state=STATE_0; state_restore=STATE_NO; }
 }
 else //=====button pressed=====================================
 { 
  //-----state after button released----------------------------
  if(state<STATE_1) state_restore=STATE_0;
  else
  { if(state<STATE_2) state_restore=STATE_1;
    else state_restore=STATE_2;
  }
  //-----both LEDs light up------------------------------------
  led_red.on();                             // led red
  led_green.on();                           // led green
  display_.screen221(3,"LED gr\x81n",'c');  // oled green
  display_.screen221(4,"LED rot",'c');      // oled red  
 }
 delay(100);
}
