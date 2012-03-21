#include "xEffecteur.h"

xEffecteur::xEffecteur(xEffecteur* driver, AssocPeriphOdid* assoc)
{
	this->driver = driver;
	assoc->add(driver);
}

xEffecteur::~xEffecteur() { }

bool xEffecteur::set(int value)
{
	return driver->set(value);
}	

