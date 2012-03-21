#include "Led.h"
#include <iostream>
Led::Led(int odid, int pin) : Effecteur(odid) 
{ 
	this->pin = pin; 
}

bool Led::set(int val)
{
	cmd[0] = val % 0x10000;
	return operation();
}

bool Led::test() { return true; };

