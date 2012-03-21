#ifndef GAOPCAPTEUR
#define GAOPCAPTEUR
#include "AbstractCapteur.h"

class GeneriqueCapteur : public AbstractCapteur
{
	public:
		//utilisation de pointeur pour wrapper sous urbi (eventuellement)
		GeneriqueCapteur(Capteur*, AssocPeriphOdid*);
		~GeneriqueCapteur();
		int get(); //doit etre implementer sur l'arduino par les objet qui herite de Capteur
		bool test();
};

#endif /*GAOPCAPTEUR*/
