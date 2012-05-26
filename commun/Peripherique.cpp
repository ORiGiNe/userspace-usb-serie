#include "Peripherique.h"
#include <stdlib.h>		// NULL

// Ne compile pas sans cette fonction
//*
#if IAmNotOnThePandaBoard
void __cxa_pure_virtual() { };
#endif
//*/
Peripherique::Peripherique(octet _odid)
{ 
	odid = _odid;
	g = NULL;
}

octet Peripherique::getOdid()
{
	return odid;
}

void Peripherique::associe(AbstractGaop* _g)
{
	g = _g;
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
	return (g == NULL) ? false : g->send(cmd, odid);
}
