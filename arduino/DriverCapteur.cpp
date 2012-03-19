#include "DriverCapteur.h"
DriverCapteur::DriverCapteur(int odid) : Peripherique(odid)
{
}

DriverCapteur::~DriverCapteur() { }

bool DriverCapteur::test()
{
	//TODO : trouver une methode pour tester
	return true;
}

void DriverCapteur::Receive(Commande& c)
{
	cmd[0] = get();
	operation(); //antwort
}

