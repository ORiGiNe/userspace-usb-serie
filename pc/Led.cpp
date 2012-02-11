#include "Led.h"
#include <iostream>
Led::Led(octet odid, octet pin) : Peripherique(odid) { this->pin = pin; }
void Led::allume()
{
	if (g == NULL) return;
	Commande cmd;
	cmd.add((octet*)"up", 2);
	g->Send(cmd, odid);
	cmd.remove();
}

bool Led::test() { return true; };

void Led::eteint()
{
	if (g == NULL) return;
	Commande cmd;
	cmd.add((octet*)"of", 2);
	g->Send(cmd, odid);
	cmd.remove();
}

void Led::Receive(Commande& c)
{
	std::cout << "led" << (int)odid << ' ' << c[0][0] << c[0][1] << std::endl;
}
