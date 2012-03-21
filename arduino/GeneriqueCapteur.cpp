#include "GeneriqueCapteur.h"

GeneriqueCapteur::GeneriqueCapteur(Capteur* driver, AssocPeriphOdid* assoc)
{
	this->driver = driver;
	assoc->add(driver);
}

GeneriqueCapteur::~GeneriqueCapteur() { }

int GeneriqueCapteur::get()
{
	return driver->get();
}	

bool GeneriqueCapteur::test()
{
	return true;
}

