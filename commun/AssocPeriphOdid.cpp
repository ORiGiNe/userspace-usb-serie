#include "AssocPeriphOdid.h"
#include "origine_debug.h"

AssocPeriphOdid::AssocPeriphOdid()
{
	taille = 0;
}

void AssocPeriphOdid::add(Peripherique *p)
{
	taille++;
	t[taille-1] = p;

	ORIGINE_DEBUG_STDOUT("DEBUG AssocPeriphOdid::add odid periph : %d" << (int)(p->getOdid()))
}

Peripherique* AssocPeriphOdid::operator[](int n)
{
	return (n >= 0 &&  n < taille) ? t[n] : NULL;
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
	int i = 0;
	for (i = 0; i < taille; i++)
	{
		if (t[i]->getOdid() == odid)
		{
			t[i] = NULL;
			break;
		}
	}
	//compact the array so getNbDevices is REALLY the nb of devices
	for(i; i < taille-1; i++)
	{
		t[i] = t[i+1];
	}
	taille--;
}

int AssocPeriphOdid::getNbDevices()
{
	return taille;
}
