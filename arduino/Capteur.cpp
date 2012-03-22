#include "Capteur.h"
Capteur::Capteur(int odid, DriverCapteur *driver) : Peripherique(odid)
{
	this->driver = driver
}

Capteur::~Capteur() { };

void Capteur::Receive(Commande& c)
{
	cmd[0] = driver->get();
	operation(); //antwort
}

int Capteur::get()
{
	return driver->get();
}

