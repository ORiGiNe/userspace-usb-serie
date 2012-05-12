#ifndef ABSTRACTCAPTEUR_H
#define ABSTRACTCAPTEUR_H

#include "Peripherique.h"
#include "DriverCapteur.h"

class AbstractCapteur : public Peripherique 
{
	public:
		//TODO Voir pour directement appeler le constructeur dans
		// la classe Capteur
		AbstractCapteur(int odid) : Peripherique(odid) { };
		virtual int get() = 0;
		virtual void receive(Commande&) = 0;
	protected:
		DriverCapteur* driver;
};

#endif /*ABSTRACTCAPTEUR*/

