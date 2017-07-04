//_____D1_oop16_clearSign.ino_________________170626-170626_____
// Deletes basicserverKH sign in EEPROM.
// Hardware: WeMos D1 Mini
#include "D1_class_BasicserverKH.h"
BasicserverKH server(80);

//_____setup routine____________________________________________
void setup(void){
 server.unsignEeprom();
}

//_____main loop________________________________________________
void loop(void){
}
