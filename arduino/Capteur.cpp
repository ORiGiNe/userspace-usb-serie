#include "Capteur.h"
Capteur::Capteur(int odid, DriverCapteur *driver) : AbstractCapteur(odid)
{
	this->driver = driver;
}


void Capteur::Receive(Commande& c)
{
	cmd[0] = driver->get();
	operation(); //antwort
}

int Capteur::get()
{
	return driver->get();
}

