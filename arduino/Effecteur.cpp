#include "Effecteur.h"

Effecteur::Effecteur(octet odid, DriverEffecteur *driver) : AbstractEffecteur(odid)
{
	this->driver = driver;
}

void Effecteur::receive(Commande& c)
{
	driver->set(c[0]);
}

// FIXME Intérêt de cette méthode ?
bool Effecteur::set(int val)
{
	return driver->set(val);
}
