#include "Commande.h"

Commande::Commande()
{
	tbl = NULL;
	taille = 0;
}

Commande::~Commande()
{
	if (tbl) free(tbl);
}

short int& Commande::operator[](unsigned int i)
{
	if (i >= taille && (sizeof(short int))*i+INFOCPL < TAILLE_MAX_FRAME) 
	{ //on suppose alors que l'on veut faire une affectation
		tbl = (short int *)realloc(tbl, (i + 1)*sizeof(short int));
		taille = i + 1;
	}
	return tbl[i];
}

int Commande::getTaille() const
{
	return taille;
}

