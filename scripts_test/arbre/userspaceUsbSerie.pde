#include "ArduinoGaop.h"
#include "Effecteur.h"
#include "Capteur.h"
#include "Led.h"
#include "Wire.h"
#include "Peripherique.h"
#include "TriggerEcho.h"
#include <WProgram.h>
#include "Servo.h"
#include "I2C.h"
#include <Wire.h>

//Warning : on ne peut pas faire de new ou de delete (mais on peut faire du malloc), ni user de la STL
//Warning : a virtual destructor requires the delete operator which in his turn requires stdlibc++ wich is not supported by avr-g++... (http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?action=print;num=1209026667) 
ArduinoGaop brain;
AssocPeriphOdid tblassoc;

TriggerEcho uuson1(6,7);
TriggerEcho uuson2(5,8);
TriggerEcho uuson3(4,9);
TriggerEcho uuson4(3,10);

I2C uuson5(0xF6);
I2C uuson6(0xF7);
I2C uuson7(0xFC);

Capteur uson1(1, &uuson1);
Capteur uson2(2, &uuson2);
Capteur uson3(3, &uuson3);
Capteur uson4(4, &uuson4);
Capteur uson5(5, &uuson5);
Capteur uson6(6, &uuson6);
Capteur uson7(7, &uuson7);

void setup()
{
	Wire.begin();
	tblassoc.add(&uson1);
	tblassoc.add(&uson2);
	tblassoc.add(&uson3);
	tblassoc.add(&uson4);
	tblassoc.add(&uson5);
	tblassoc.add(&uson6);
	tblassoc.add(&uson7);
	brain.initialise(&tblassoc);
}

void loop()
{
	brain.receive(tblassoc);
}
