#include "Peripherique.h"
#include <stdlib.h>

Peripherique::Peripherique(octet odid) 
{ 
	this->odid = odid;
	this->g = NULL;
}

octet Peripherique::getOdid()
{
	return odid;
}

void Peripherique::associe(AbstractGaop* g)
{
	this->g = g;
} 

// TODO evolution future
//test le fonctionnenment du peripherique, et renvoie faux si il ne marche pas
bool Peripherique::test()
{
	return true;
} 

/**
	Effectue l'operation a l'aide de la methode Send de GAOP
*/
bool Peripherique::operation()
{
	if (g == NULL)
		return false;
	
	bool ret = g->send(cmd, odid);
	return ret;
}

//void Peripherique::Receive(Commande& c){  }


