#include "Commande.h"

Commande::Commande()
{
	taille = 0;
}

short int& Commande::operator[](unsigned int i)
{
	return (i < TBL_MAX) ? tbl[i] : -1;
}

int Commande::getTaille() const
{
	return taille;
}

void Commande::setTaille(int _taille)
{
	taille = _taille;
}
