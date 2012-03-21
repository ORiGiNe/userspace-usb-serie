#include "GeneriqueEffecteur.h"

GeneriqueEffecteur::GeneriqueEffecteur(Effecteur* driver, AssocPeriphOdid* assoc)
{
	this->driver = driver;
	assoc->add(driver);
}

GeneriqueEffecteur::~GeneriqueEffecteur() { }

bool GeneriqueEffecteur::set(int value)
{
	return driver->set(value);
}	

bool GeneriqueEffecteur::test()
{
	return true;
}

