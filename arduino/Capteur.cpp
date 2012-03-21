#include "Capteur.h"
Capteur::Capteur(int odid) : Peripherique(odid)
{
}

Capteur::~Capteur() { }

void Capteur::Receive(Commande& c)
{
	cmd[0] = get();
	operation(); //antwort
}

