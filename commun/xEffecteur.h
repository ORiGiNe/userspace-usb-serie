#ifndef GAOPEFFECTEUR
#define GAOPEFFECTEUR
#include "AbstractEffecteur.h"

class Effecteur : public AbstractEffecteur
{
	public:
		//utilisation de pointeur pour wrapper sous urbi (eventuellement)
		Effecteur(DriverEffecteur*, AssocPeriphOdid*);
		~Effecteur();
		bool set(int val); //doit etre implementer sur l'arduino par les objet qui herite de DriverEffecteur
};

#endif /*GAOPEFFECTEUR*/
