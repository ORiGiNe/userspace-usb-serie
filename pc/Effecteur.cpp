#include "Effecteur.h"

Effecteur::Effecteur(int oidi) : Peripherique(oidi)
{

}

void Effecteur::Receive(Commande& c)
{

}

bool Effecteur::set(int o)
{
		cmd[0] = o % 0x00010000;
		return operation();
}

