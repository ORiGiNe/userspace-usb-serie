#include "DriverEffecteur.h"

DriverEffecteur::DriverEffecteur(int odid, int lim_bas, int lim_haut) : Peripherique(odid)
{
	valInf = lim_bas;
	valSup = lim_haut;
}

DriverEffecteur::~DriverEffecteur() { }

bool DriverEffecteur::test()
{
	//TODO : trouver une methode pour tester
	return true;
}

void DriverEffecteur::Receive(Commande& c)
{
	if (c[0] >= valInf && c[0] <= valSup) set(c[0]);
}

