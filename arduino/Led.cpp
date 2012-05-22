#include "Led.h"

Led::Led(int _pin)
{
	pin = _pin;
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
