#ifndef GAOPCAPTEUR_H
#define GAOPCAPTEUR_H

#include "AbstractCapteur.h"

class Capteur : public AbstractCapteur
{
	public:
		Capteur(int odid, DriverCapteur*);
		int get();
		void receive(Commande&);
};

#endif /*GAOPCAPTEUR*/
