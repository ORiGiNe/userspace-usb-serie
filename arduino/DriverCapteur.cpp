#include "DriverCapteur.h"
DriverCapteur::DriverCapteur(int odid) : Peripherique(odid)
{
}

DriverCapteur::~DriverCapteur() { }

void DriverCapteur::Receive(Commande& c)
{
	cmd[0] = get();
	operation(); //antwort
}

