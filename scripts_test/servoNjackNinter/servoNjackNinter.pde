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


int pin1[1] = {5};
int pin2[1] = {6};
int pin3[1] = {9};
int pin4[1] = {10};

EffecteurServo sservo1(pin1,1);
Effecteur servo1(11, &sservo1);

EffecteurServo sservo2(pin2,1);
Effecteur servo2(12, &sservo2); 

EffecteurServo sservo3(pin3,1);
Effecteur servo3(13, &sservo3);

EffecteurServo sservo4(pin4,1);
Effecteur servo4(14, &sservo4);

Jack jjack(2);
Capteur jack(21, &jjack);

Jack iinterCouleur(3);
Capteur interCouleur(22, &iinterCouleur);


void setup()
{
	
        tblassoc.add(&jack);
	tblassoc.add(&interCouleur);
        

	sservo1.setup();
	sservo2.setup();
	sservo3.setup();
	sservo4.setup();
	tblassoc.add(&servo1);	
        tblassoc.add(&servo2);
	tblassoc.add(&servo3);	
        tblassoc.add(&servo4);

	brain.initialise(&tblassoc); //Serial.begin(115200);
}

void loop()
{
	brain.receive(tblassoc);
}

