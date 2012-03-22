#ifndef GAOPEFFECTEUR
#define GAOPEFFECTEUR

#include "AbstractEffecteur.h"

class Effecteur : public AbstractEffecteur
{
	public:
		Effecteur(int odid,  DriverEffecteur*);
		~Effecteur();
		bool set(int valeur);
		void Receive(Commande&);
};

#endif /* GAOPEFFECTEUR */
