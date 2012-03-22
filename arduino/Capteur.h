#ifndef GAOPCAPTEUR
#define GAOPCAPTEUR

#include "AbstractCapteur.h"

class Capteur : public AbstractCapteur
{
	public:
		Capteur(int odid, DriverCapteur*);
		~Capteur();
		int get(); 
		void Receive(Commande&);
};

#endif /*GAOPCAPTEUR*/
