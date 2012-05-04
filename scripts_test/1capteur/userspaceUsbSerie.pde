#include "GaopProtocol.h"
#include "Effecteur.h"
#include "Peripherique.h"
#include "TriggerEcho.h"

Gaop brain;

TriggerEcho uuson1(3, 4); //trigger = 3, echo = 4 (pin)
AssocPeriphOdid tblassoc;

Capteur uson1(1, &uuson1);

void setup()
{
	tblassoc.add(&uson1);
	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.Receive(tblassoc);
}

