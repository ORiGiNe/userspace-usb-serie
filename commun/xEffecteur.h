#ifndef GAOPEFFECTEUR
#define GAOPEFFECTEUR
#include "AbstractEffecteur.h"

class xEffecteur : public AbstractEffecteur
{
	public:
		//utilisation de pointeur pour wrapper sous urbi (eventuellement)
		xEffecteur(Effecteur*, AssocPeriphOdid*);
		~xEffecteur();
		bool set(int val); //doit etre implementer sur l'arduino par les objet qui herite de DriverEffecteur
};

#endif /*GAOPEFFECTEUR*/
