#include "Effecteur.h"

Effecteur::Effecteur(int odid, DriverEffecteur *driver) : AbstractEffecteur(odid)
{
	this->driver = driver;
}

void Effecteur::Receive(Commande& c)
{
	driver->set(c[0]);
}

// FIXME Intérêt de cette méthode ?
bool Effecteur::set(int val)
{
	return driver->set(val);
}

