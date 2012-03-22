#ifndef ABSTRACTCAPTEUR
#define ABSTRACTCAPTEUR

#include "Commande.h"
#include "Peripherique.h"
#include "DriverCapteur.h"

class AbstractCapteur : public Peripherique
{
	public:
		virtual int get() = 0;
		virtual void Receive(Commande&) = 0;
	protected:
		DriverCapteur* driver;
};

#endif /*ABSTRACTCAPTEUR*/

