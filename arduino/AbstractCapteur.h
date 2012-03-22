#ifndef ABSTRACTCAPTEUR
#define ABSTRACTCAPTEUR

#include "Commande.h"
#include "DriverCapteur.h"

/* C'est une interface. On n'herite pas de Peripherique */
class AbstractCapteur 
{
	public:
		virtual int get() = 0;
		virtual void Receive(Commande&) = 0;
	protected:
		DriverCapteur* driver;
};

#endif /*ABSTRACTCAPTEUR*/

