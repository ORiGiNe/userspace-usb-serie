#ifndef ABSTRACTEFFECTEUR_H
#define ABSTRACTEFFECTEUR_H

#include "Peripherique.h"
#include "DriverEffecteur.h" 

class AbstractEffecteur : public Peripherique
{
	public:
		AbstractEffecteur(octet odid) : Peripherique(odid) { };
		virtual bool set(int valuetoset) = 0;
		virtual void receive(Commande&) = 0;
	protected:
		DriverEffecteur* driver;
};

#endif
