#include "GaopProtocol.h"
#include "Effecteur.h"
#include "Capteur.h"
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
Led lled1(6);

//TriggerEcho uuson1(3, 4); //trigger = 3, echo = 4 (pin)
I2c uuson1(0xF0);
AssocPeriphOdid tblassoc;

Effecteur led1(0, &lled1);
Capteur uson1(1, &uuson1);

//*/
TriggerEcho uuson1(2, 3); //trigger = 2, echo = 3 (pin)
Capteur uson(4,&uuson1);

int caca[3] = {9,10,11};
EffecteurServo sservo1(caca,3);
Effecteur servo1(2, &sservo1);
void setup()
{
/*
	tblassoc.add(&led1); //doit etre fait pour tout les devices
	tblassoc.add(&uson1);
//*/
	sservo1.setup();
	tblassoc.add(&servo1);
	tblassoc.add(&uson);
	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.Receive(tblassoc);
}

