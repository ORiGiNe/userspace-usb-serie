#ifndef GAOPEFFECTEUR
#define GAOPEFFECTEUR

#include "Peripherique.h"

class Effecteur : public Peripherique
{
	public:
		Effecteur(int odid);
		bool set(int valeur); //renvoie faux si echec de l'envoie de la valeur.
		void Receive(Commande&); //nothing
};

#endif /* GAOPEFFECTEUR */
