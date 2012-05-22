#include "Effecteur.h"

Effecteur::Effecteur(octet odid, DriverEffecteur *_driver) : AbstractEffecteur(odid)
{
	driver = _driver;
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
