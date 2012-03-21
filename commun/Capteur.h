#ifndef GAOPCAPTEUR
#define GAOPCAPTEUR
#include "AbstractCapteur.h"

class Capteur : public AbstractCapteur
{
	public:
		//utilisation de pointeur pour wrapper sous urbi (eventuellement)
		Capteur(Capteur*, AssocPeriphOdid*);
		~Capteur();
		int get(); //doit etre implementer sur l'arduino par les objet qui herite de Capteur
		bool test();
};

#endif /*GAOPCAPTEUR*/
