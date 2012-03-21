#include "Effecteur.h"

Effecteur::Effecteur(int oidi, int val_inf, int val_sup) : Peripherique(oidi)
{
	valInf = val_inf;
	valSup = val_sup;
}
Effecteur::~Effecteur() { };

void Effecteur::Receive(Commande& c)
{

}

bool Effecteur::set(int o)
{
	if (o >= valInf && o <= valSup)
	{
		cmd[0] = o % 0x00010000;
		return operation();
	} else return false;
}

