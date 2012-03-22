#include "GaopProtocol.h"
#include "GeneriqueEffecteur.h"
#include "Led.h"

//Warning : on ne peut pas faire de new ou de delete (mais on peut faire du malloc), ni user de la STL
//Warning : a virtual destructor requires the delete operator which in his turn requires stdlibc++ wich is not supported by avr-g++... (http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?action=print;num=1209026667) 
Gaop brain;
Led lled1(6);
AssocPeriphOdid tblassoc;
Effecteur led1(0, &lled);
tblassoc.add(&led1); //doit etre fait pour tout les devices

void setup()
{
	/**///pinMode(13, OUTPUT);	
	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.Receive(tblassoc);
}

