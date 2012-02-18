#include "GaopProtocol.h"
#include "Led.h"

//Warning : on ne peut pas faire de new ou de delete (mais on peut faire du malloc), ni user de la STL
Gaop brain;
Led led1(0, 6);
Led led2(1, 9);
AssocPeriphOdid tblassoc;

void setup()
{
	/**///pinMode(13, OUTPUT);	
	tblassoc.add(&led1);
	tblassoc.add(&led2);
	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.Receive(tblassoc);
}

