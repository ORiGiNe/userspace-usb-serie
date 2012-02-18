#include "Led.h"

Led::Led(int odid, int pin) : Peripherique(odid) 
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
	if (c[0][0] == 'u') 
	{
		allume();
		cmd.add((octet*)"on", 2);
	}
	else 
	{
		eteint();
		cmd.add((octet*)"--", 2);
	}
	operation(); //envoie une reponse
}
