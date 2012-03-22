#ifndef ABSTRACTEFFECTEUR
#define ABSTRACTEFFECTEUR

#include "Commande.h"
#include "Peripherique.h"
#include "DriverEffecteur.h" 

class AbstractEffecteur : public Peripherique
{
	public:
		virtual bool set(int valuetoset) = 0;
    	virtual void Receive(Commande&) = 0;
	protected:
		DriverEffecteur* driver;
};

#endif
