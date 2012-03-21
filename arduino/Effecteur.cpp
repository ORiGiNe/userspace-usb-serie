#include "Effecteur.h"

Effecteur::Effecteur(int odid, int lim_bas, int lim_haut) : Peripherique(odid)
{
	valInf = lim_bas;
	valSup = lim_haut;
}

Effecteur::~Effecteur() { }

void Effecteur::Receive(Commande& c)
{
	if (c[0] >= valInf && c[0] <= valSup) set(c[0]);
}

