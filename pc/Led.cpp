#include "Led.h"
#include <iostream>
Led::Led(int odid, int pin) : Peripherique(odid) { this->pin = pin; }
void Led::allume()
{
	cmd[0] = 'u';
	operation();
}

bool Led::test() { return true; };

void Led::eteint()
{
	cmd[0] = 'o';
	operation();
}

void Led::Receive(Commande& c)
{
	std::cout << "led" << (int)odid << ' ' << (char)c[0] << std::endl;
}
