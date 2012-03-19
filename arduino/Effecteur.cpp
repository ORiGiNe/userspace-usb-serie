#include "Effecteur.h"

Effecteur::Effecteur(DriverEffecteur* driver, AssocPeriphOdid* assoc)
{
	this->driver = driver;
	assoc->add(driver);
}

Effecteur::~Effecteur() { }

bool Effecteur::set(int value)
{
	return driver->set(value);
}	

