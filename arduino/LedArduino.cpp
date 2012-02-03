#include "Led.h"

Led::Led(octet odid) : Peripherique(odid) 
{
	pinMode(odid, OUTPUT); //on utilise l'odid comme pin.
}
void Led::allume()
{
	digitalWrite(odid, HIGH);
}	
void Led::eteint()
{
	digitalWrite(odid, LOW);
}
bool Led::test()
{
	//1 court
	digitalWrite(odid, HIGH);
	delay(300);
	digitalWrite(odid, LOW);
	return true;
}

void Led::Receive(Commande &c)
{
	Commande commande;
	if (c[0][0] == 'u') 
	{
		allume();
		commande.add((octet*)"on", 2);
	}
	else 
	{
		eteint();
		commande.add((octet*)"--", 2);
	}
	g->Send(commande, odid); //sending an answer
}
