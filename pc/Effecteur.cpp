#include "DriverEffecteur.h"

DriverEffecteur::DriverEffecteur(int oidi, int val_inf, int val_sup) : Peripherique(oidi)
{
	valInf = val_inf;
	valSup = val_sup;
}
DriverEffecteur::~DriverEffecteur() { };

void DriverEffecteur::Receive(Commande& c)
{

}

bool DriverEffecteur::set(int o)
{
	if (o >= valInf && o <= valSup)
	{
		cmd[0] = o % 0x00010000;
		return operation();
	} else return false;
}

