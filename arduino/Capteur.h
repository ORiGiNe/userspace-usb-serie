#ifndef GAOPCAPTEUR
#define GAOPCAPTEUR

#include "AbstractCapteur.h"
#include "Peripherique.h"

class Capteur : public AbstractCapteur, public Peripherique
{
	public:
		Capteur(int odid, DriverCapteur*);
		int get(); 
		void Receive(Commande&);
};

#endif /*GAOPCAPTEUR*/
