#include "Capteur.h"
Capteur::Capteur(int odid) : Peripherique(odid)
{
}

Capteur::~Capteur() { }

bool Capteur::test()
{
	//TODO : trouver une methode pour tester
	return true;
}

void Capteur::Receive(Commande& c)
{
	cmd[0] = get();
	operation(); //antwort
}

