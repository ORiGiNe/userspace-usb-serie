#include "EffecteurServo.h"
#include <Servo.h>

EffecteurServo::EffecteurServo(int _pin[], int _nbr)
{
	for (int i = 0; i < _nbr; i++)
	{
		pin[i] = _pin[i];
	}

	pos = 0;
	nbr = _nbr;
}

bool EffecteurServo::setup()
{
	for (int i = 0; i < this->nbr; i++)
	{
		myservo[i].attach(pin[i]); //attaches the servo on pin to the servo object (pin 14 = analog 0);
	}
}

bool EffecteurServo::set(int val)
{
	if ( val < 0 || val > 180 )
		return false;

	myservo[i].write(val);
	pos = val;

	return true;
}
