#include "Capteur.h"

Capteur::Capteur(Capteur* driver, AssocPeriphOdid* assoc)
{
	this->driver = driver;
	assoc->add(driver);
}

Capteur::~Capteur() { }

int Capteur::get()
{
	return driver->get();
}	

bool Capteur::test()
{
	return true;
}

