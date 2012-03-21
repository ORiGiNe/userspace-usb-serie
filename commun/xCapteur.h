#ifndef GAOPCAPTEUR
#define GAOPCAPTEUR
#include "AbstractCapteur.h"

class xCapteur : public AbstractCapteur
{
	public:
		//utilisation de pointeur pour wrapper sous urbi (eventuellement)
		xCapteur(Capteur*, AssocPeriphOdid*);
		~xCapteur();
		int get(); //doit etre implementer sur l'arduino par les objet qui herite de DriverCapteur
};

#endif /*GAOPCAPTEUR*/
