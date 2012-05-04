#include "GaopProtocol.h"
#include "Effecteur.h"
#include "Capteur.h"
#include "Led.h"
#include "Peripherique.h"
#include "TriggerEcho.h"

//Warning : on ne peut pas faire de new ou de delete (mais on peut faire du malloc), ni user de la STL
//Warning : a virtual destructor requires the delete operator which in his turn requires stdlibc++ wich is not supported by avr-g++... (http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?action=print;num=1209026667) 
Gaop brain;

Led lled1(6);
TriggerEcho uuson1(3, 4); //trigger = 3, echo = 4 (pin)
TriggerEcho uuson2(9, 10); //trigger = 9, echo = 10 (pin)
AssocPeriphOdid tblassoc;

Effecteur led1(0, &lled1);
Capteur uson1(1, &uuson1);
Capteur uson2(2, &uuson1);

void setup()
{
	tblassoc.add(&led1); //doit etre fait pour tout les devices
	tblassoc.add(&uson1);
	tblassoc.add(&uson2);
	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.Receive(tblassoc);
}

