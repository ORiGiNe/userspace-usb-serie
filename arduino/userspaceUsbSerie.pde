#include "GaopProtocol.h"
#include "Effecteur.h"
#include "Led.h"

//Warning : on ne peut pas faire de new ou de delete (mais on peut faire du malloc), ni user de la STL
Gaop brain;
Led driverLed1(0, 6);
Led driverLed2(1, 9);
AssocPeriphOdid tblassoc;
Effecteur led1(&driverLed1, &tblassoc);
Effecteur led2(&driverLed2, &tblassoc);

void setup()
{
	/**///pinMode(13, OUTPUT);	
	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.Receive(tblassoc);
}

