#ifndef GAOPEFFECTEUR
#define GAOPEFFECTEUR
#include "AbstractEffecteur.h"

class GeneriqueEffecteur : public AbstractEffecteur
{
	public:
		//utilisation de pointeur pour wrapper sous urbi (eventuellement)
		GeneriqueEffecteur(Effecteur*, AssocPeriphOdid*);
		~GeneriqueEffecteur();
		bool test();
		bool set(int val); //doit etre implementer sur l'arduino par les objet qui herite de Effecteur
};

#endif /*GAOPEFFECTEUR*/
