#include "Led.h"
#include <iostream>
Led::Led(int odid, int pin) : Peripherique(odid) { this->pin = pin; }
void Led::allume()
{
	cmd.add((octet*)"up", 2);
	operation();
}

bool Led::test() { return true; };

void Led::eteint()
{
	cmd.add((octet*)"of", 2);
	operation();
}

void Led::Receive(Commande& c)
{
	std::cout << "led" << (int)odid << ' ' << c[0][0] << c[0][1] << std::endl;
}
