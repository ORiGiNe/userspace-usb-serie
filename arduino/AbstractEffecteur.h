#ifndef ABSTRACTEFFECTEUR
#define ABSTRACTEFFECTEUR

#include "Commande.h"
#include "DriverEffecteur.h" 

/* C'est une interface. On n'herite pas de Peripherique */
class AbstractEffecteur
{
	public:
		virtual bool set(int valuetoset) = 0;
    	virtual void Receive(Commande&) = 0;
	protected:
		DriverEffecteur* driver;
};

#endif
