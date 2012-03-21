#include "Capteur.h"

Capteur::Capteur(DriverCapteur* driver, AssocPeriphOdid* assoc)
{
	this->driver = driver;
	assoc->add(driver);
}

Capteur::~Capteur() { }

int Capteur::get()
{
	return driver->get();
}	

