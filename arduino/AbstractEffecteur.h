#ifndef ABSTRACTEFFECTEUR
#define ABSTRACTEFFECTEUR

#include "Peripherique.h"
#include "Commande.h"
#include "DriverEffecteur.h" 

class AbstractEffecteur : public Peripherique
{
	public:
		AbstractEffecteur(int odid) : Peripherique(odid) { };
		virtual bool set(int valuetoset) = 0;
    	virtual void Receive(Commande&) = 0;
	protected:
		DriverEffecteur* driver;
};

#endif
