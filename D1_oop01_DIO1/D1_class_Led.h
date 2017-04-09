//_____D1_class_Led.h_________________________170402-170402_____
// D1 mini class for a led. Default is blue led (D4=GPIO2)
//    Difference to digital out: 0=led on, 1=led off
#ifndef D1_CLASS_LED_H
#define D1_CLASS_LED_H

class Led {
 private:
  void setup(int num);
  int led_num;
 public:
  Led();
  Led(int num);
  void set(int on_0);
  void on(void);
  void off(void);
};

//_____constructor (default button is D3)_______________________
Led::Led() { setup(D4); }

//_____constructor with button number (=GPIO)___________________
Led::Led(int num) { setup(num); }

void Led::setup(int num)
{
 led_num=num;
 pinMode(led_num, OUTPUT);        // set io to output
 this->off();
}

//_____set led io_______________________________________________
void Led::set(int on_0)  { digitalWrite(led_num, on_0); }

//_____switch led on____________________________________________
void Led::on(void)  { digitalWrite(led_num, 0); }

//_____switch led off___________________________________________
void Led::off(void) { digitalWrite(led_num, 1); }
#endif
