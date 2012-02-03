#include "Led.h"
#include <iostream>
Led::Led(octet odid) : Peripherique(odid) { }
void Led::allume()
{
	if (g == NULL) return;
	octet odid_r = 0;
	Commande cmd;
	cmd.add((octet*)"up", 2);
	g->Send(cmd, odid);
	cmd.remove();
	//tant que ça ne nous est pas destinee
	while (odid_r != odid) g->Receive(cmd, odid_r); 
	Receive(cmd);
}

bool Led::test() { return true; };

void Led::eteint()
{
	if (g == NULL) return;
	octet odid_r = 0;
	Commande cmd;
	cmd.add((octet*)"of", 2);
	g->Send(cmd, odid);
	cmd.remove();
	//tant que ça ne nous est pas destinee
	while (odid_r != odid) g->Receive(cmd, odid_r);
	Receive(cmd);
}

void Led::Receive(Commande& c)
{
//	std::cout << "led" << (int)odid << ' ' << c[0][0] << c[0][1] << std::endl;
}
