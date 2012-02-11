#include "Led.h"

Led::Led(octet odid, octet pin) : Peripherique(odid) 
{
	this->pin = pin;
	pinMode(pin, OUTPUT); //on utilise l'odid comme pin.
}
void Led::allume()
{
	digitalWrite(pin, HIGH);
}	
void Led::eteint()
{
	digitalWrite(pin, LOW);
}
bool Led::test()
{
	//1 court
	digitalWrite(pin, HIGH);
	delay(300);
	digitalWrite(pin, LOW);
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
