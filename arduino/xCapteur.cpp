#include "xCapteur.h"

xCapteur::xCapteur(Capteur* driver, AssocPeriphOdid* assoc)
{
	this->driver = driver;
	assoc->add(driver);
}

xCapteur::~xCapteur() { }

int xCapteur::get()
{
	return driver->get();
}	

