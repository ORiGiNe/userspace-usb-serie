#include "Effecteur.h"

Effecteur::Effecteur(int oidi) : Peripherique(oidi)
{

}

void Effecteur::receive(Commande& c)
{

}

bool Effecteur::set(int o)
{
	// Pour urbi, qui ne gère pas les short int, mais seulement les ints
	cmd[0] = o % 0x00010000;
	return operation();
}

