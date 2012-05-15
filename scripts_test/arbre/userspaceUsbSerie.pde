#include "GaopProtocol.h"
#include "Effecteur.h"
#include "Capteur.h"t first I tried with bigger addresses, like FC, so maybe a bit is shifted and the result gets out of the expected range 
#include "Led.h"
#include "Wire.h"
#include "Peripherique.h"
#include "TriggerEcho.h"
#include "ClassServo.h"
#include <WProgram.h>
#include "Servo.h"
#include "I2C.h"
#include <Wire.h>

//Warning : on ne peut pas faire de new ou de delete (mais on peut faire du malloc), ni user de la STL
//Warning : a virtual destructor requires the delete operator which in his turn requires stdlibc++ wich is not supported by avr-g++... (http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?action=print;num=1209026667) 
Gaop brain;
AssocPeriphOdid tblassoc;

/*
exemples :
	TriggerEcho uuson1(3, 4); //trigger = 3, echo = 4 (pin)
	I2c uuson1(0xF0);
	Capteur uson1(1, &uuson1);
	int pin1[1] = {2};
	EffecteurServo sservo1(pin1, 1);
	Effecteur servo1(2, &sservo1);
*/

I2c uusonRouge(0xF6);
I2c uusonNoir(0xF7);
I2c uusonVert(0xFC);
TriggerEcho uuson1 (6,7);
TriggerEcho uuson2 (5,8);
TriggerEcho uuson3 (4,9);
TriggerEcho uuson4 (3,10);


Capteur usonRouge(5, &uusonRouge);
Capteur usonNoir(7, &uusonNoir);
Capteur usonVert(6, &uusonVert);
Capteur uson1(1, &uuson1);
Capteur uson2(2, &uuson2);
Capteur uson3(3, &uuson3);
Capteur uson4(4, &uuson4);


void setup()
{
/*
	tblassoc.add(&led1); //doit etre fait pour tout les devices
	tblassoc.add(&uson1);
	sservo1.setup();
	tblassoc.add(&servo1);
*/
	Wire.begin();
	tblassoc.add(&usonRouge);
	tblassoc.add(&usonNoir);
	tblassoc.add(&usonVert);
	tblassoc.add(&uson1);
	tblassoc.add(&uson2);
	tblassoc.add(&uson3);
	tblassoc.add(&uson4);
	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.Receive(tblassoc);
}

