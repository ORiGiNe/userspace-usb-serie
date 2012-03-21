#include "Led.h"

Led::Led(int odid, int pin) : Effecteur(odid) 
{
	this->pin = pin;
	pinMode(pin, OUTPUT); 
}

bool Led::set(int val)
{
	if (val == 0)
		digitalWrite(pin, LOW);
	else
		digitalWrite(pin, HIGH);
	return true;
}

bool Led::test()
{
	//1 court
	digitalWrite(pin, HIGH);
	delay(300);
	digitalWrite(pin, LOW);
	return true;
}

