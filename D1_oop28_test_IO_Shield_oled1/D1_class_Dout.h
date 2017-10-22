//_____D1_class_Dout.h________________________170402-170402_____
// D1 mini class for digital output, e.g. leds.
// Default is blue led (D4=GPIO2), not inverted output (0->0V)
#ifndef D1_CLASS_DOUT_H
#define D1_CLASS_DOUT_H

class Dout {
 private:
  int  dout_num;
  bool invert_;
 private:
  void setup(int num, bool invert);
 public:
  Dout();
  Dout(int num);
  Dout(int num, bool invert);
  void set(int val);
  void on(void);
  void off(void);
};

//_____constructor (default button is D3)_______________________
Dout::Dout() { setup(D4,false); }

//_____constructor with button number (=GPIO)___________________
Dout::Dout(int num) { setup(num,false); }

//_____constructor with button number and invert________________
Dout::Dout(int num, bool invert) { setup(num,invert); }

//_____setup output pin_________________________________________
void Dout::setup(int num, bool invert)
{
 this->invert_=invert;
 dout_num=num;
 pinMode(dout_num, OUTPUT);        // set io to output
 this->off();
}

//_____set led io_______________________________________________
void Dout::set(int val)
{
 int val_=val;
 if(val_!=0) val_=1;
 if(invert_) val_=1-val_;
 digitalWrite(dout_num, val_); 
}

//_____switch led on____________________________________________
void Dout::on(void)  { this->set(1); }

//_____switch led off___________________________________________
void Dout::off(void) { this->set(0); }
#endif
