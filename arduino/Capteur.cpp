#include "Capteur.h"

Capteur::Capteur(int odid, DriverCapteur *_driver) : AbstractCapteur(odid)
{
	driver = _driver;
}


void Capteur::receive(Commande& c)
{
	cmd[0] = driver->get();
	operation(); //antwort
}

int Capteur::get()
{
	return driver->get();
}
