#ifndef GAOPEFFECTEUR
#define GAOPEFFECTEUR

#include "Peripherique.h"

class Effecteur : public Peripherique
{
	public:
		Effecteur(int odid);
		bool set(int valeur);
		void Receive(Commande&);
};

#endif /* GAOPEFFECTEUR */
