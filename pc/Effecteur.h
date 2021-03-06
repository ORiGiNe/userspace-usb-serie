#ifndef GAOPEFFECTEUR
#define GAOPEFFECTEUR

#include "Peripherique.h"

class Effecteur : public Peripherique
{
	public:
		Effecteur(octet odid);
		bool set(int valeur); //renvoie faux si echec de l'envoie de la valeur.
		void receive(Commande&); //nothing
};

#endif /* GAOPEFFECTEUR */
