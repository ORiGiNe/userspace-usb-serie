#ifndef GAOPEFFECTEUR_H
#define GAOPEFFECTEUR_H

#include "AbstractEffecteur.h"
#include "Peripherique.h"

class Effecteur : public AbstractEffecteur
{
	public:
		Effecteur(int odid,  DriverEffecteur*);
		bool set(int valeur);
		void receive(Commande&);
};

#endif /* GAOPEFFECTEUR */
