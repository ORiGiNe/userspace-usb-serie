#include "Commande.h"

Commande::Commande()
{
	taille = 0;
}

short int& Commande::operator[](unsigned int i)
{
	// FIXME beurk
	if (i < TBL_MAX && i >= taille)
		taille = i+1;

	return (i < TBL_MAX) ? tbl[i]: tbl[TBL_MAX-1];
}

int Commande::getTaille() const
{
	return taille;
}

void Commande::setTaille(int _taille)
{
	taille = _taille;
}
