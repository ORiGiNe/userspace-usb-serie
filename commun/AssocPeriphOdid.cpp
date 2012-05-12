#include "AssocPeriphOdid.h"
#include <stdlib.h>

AssocPeriphOdid::AssocPeriphOdid()
{
	this->t = NULL;
	this->taille = 0;
}

AssocPeriphOdid::~AssocPeriphOdid()
{
	if (t)
		free(t);
}

void AssocPeriphOdid::add(Peripherique *p)
{
	taille++;
	t = (Peripherique**)realloc(t, taille*sizeof(Peripherique*));
	t[taille-1] = p;
}

Peripherique* AssocPeriphOdid::operator[](int n)
{
	if (n >= 0 &&  n < taille)
		return t[n];
	else
		return NULL;
}

Peripherique* AssocPeriphOdid::getByODID(int odid)
{
	for (int i = 0; i < taille; i++)
	{
		if (t[i]->getOdid() == odid)
			return t[i];
	}
	return NULL;
}
void AssocPeriphOdid::rm(int odid)
{
	for (int i = 0; i < taille; i++)
	{
		if (t[i]->getOdid() == odid)
		{
			t[i] = NULL;
			break;
		}
	}
}

int AssocPeriphOdid::getNbDevices()
{
	return taille;
}


