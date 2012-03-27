#ifndef ABSTRACTCAPTEUR
#define ABSTRACTCAPTEUR

#include "Peripherique.h"
#include "Commande.h"
#include "DriverCapteur.h"

class AbstractCapteur : public Peripherique 
{
	public:
		//TODO Voir pour directement appeler le constructeur dans
		// la classe Capteur
		AbstractCapteur(int odid) : Peripherique(odid) { };
		virtual int get() = 0;
		virtual void Receive(Commande&) = 0;
	protected:
		DriverCapteur* driver;
};

#endif /*ABSTRACTCAPTEUR*/

