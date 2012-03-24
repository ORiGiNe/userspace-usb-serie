#ifndef GAOPEFFECTEUR
#define GAOPEFFECTEUR

#include "AbstractEffecteur.h"
#include "Peripherique.h"

class Effecteur : public AbstractEffecteur
{
	public:
		Effecteur(int odid,  DriverEffecteur*);
		bool set(int valeur);
		void Receive(Commande&);
};

#endif /* GAOPEFFECTEUR */
