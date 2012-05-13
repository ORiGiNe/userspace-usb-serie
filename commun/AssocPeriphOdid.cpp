#include "AssocPeriphOdid.h"
#include <stdlib.h>

#if DEBUG && !IAmNotOnThePandaBoard
using namespace std;
#include <iostream>
#endif

AssocPeriphOdid::AssocPeriphOdid()
{
	t = NULL;
	taille = 0;
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

#ifdef DEBUG && !IAmNotOnThePandaBoard
	cout << "DEBUG AssocPeriphOdid::add odid periph : " << (int)(p->getOdid()) <<endl;
#endif
}

Peripherique* AssocPeriphOdid::operator[](int n)
{
	if (n >= 0 &&  n < taille)
		return t[n];
	else
		return NULL;
}

Peripherique* AssocPeriphOdid::getByODID(octet odid)
{
	for (int i = 0; i < taille; i++)
	{
		if (t[i]->getOdid() == odid)
			return t[i];
	}
	return NULL;
}
void AssocPeriphOdid::rm(octet odid)
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
