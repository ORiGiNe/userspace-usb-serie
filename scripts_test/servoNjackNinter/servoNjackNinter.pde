#include "ArduinoGaop.h"
#include "Effecteur.h"
#include "Capteur.h"
#include "Wire.h"
#include "Peripherique.h"
#include "EffecteurServo.h"
#include <WProgram.h>
#include "Servo.h"
#include "Jack.h"
#include <Wire.h>

//Warning : on ne peut pas faire de new ou de delete (mais on peut faire du malloc), ni user de la STL
//Warning : a virtual destructor requires the delete operator which in his turn requires stdlibc++ wich is not supported by avr-g++... (http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?action=print;num=1209026667) 
ArduinoGaop brain;
AssocPeriphOdid tblassoc;

Jack jjack(7);
Capteur jack(1, &jjack);

Jack iinterCouleur(9);
Capteur interCouleur(2, &iinterCouleur);

int pin1[2] = {2,3};
int pin2[2] = {4,5};

EffecteurServo sservo1(pin1,1);
Effecteur servo1(3, &sservo1);

EffecteurServo sservo2(pin2,1);
Effecteur servo2(4, &sservo2);

void setup()
{
	tblassoc.add(&jack);
	tblassoc.add(&interCouleur);

	sservo1.setup();
	sservo2.setup();
	tblassoc.add(&servo1);	
        tblassoc.add(&servo2);

	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.receive(tblassoc);
}

