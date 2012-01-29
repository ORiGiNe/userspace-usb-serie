#include "Led.h"
#include <iostream>
Led::Led(octet odid, Gaop &g) : Peripherique(odid, g) { }
void Led::allume()
{
	Commande cmd;
	cmd.add((octet*)"up", 2);
	g->Send(cmd, odid);
	cmd.remove();
	g->Receive(cmd, odid);
	Receive(cmd);
}

bool Led::test() { return true; };

void Led::eteint()
{
	Commande cmd;
	cmd.add((octet*)"of", 2);
	g->Send(cmd, odid);
	cmd.remove();
	g->Receive(cmd, odid);
	Receive(cmd);
}

void Led::Receive(Commande& c)
{
	std::cout << "led" << (int)odid << ' ' << c[0][0] << c[0][1] << std::endl;
}
