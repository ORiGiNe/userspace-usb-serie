#include "Led.h"

Led::Led(int pin)
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

