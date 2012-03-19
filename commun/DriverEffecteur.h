#ifndef DRIVEREFFECTEUR
#define DRIVEREFFECTEUR

#include "Peripherique.h"

class DriverEffecteur : public	Peripherique
{
	public:
		DriverEffecteur(int odid, int valInf = -32768, int valSup = 32768);
#if IAmNotOnThePandaBoard
		~DriverEffecteur();
#else
		virtual ~DriverEffecteur();
#endif
		virtual bool set(int valeur) = 0;
		bool test();
		void Receive(Commande&);
	protected:
		int valInf;
		int valSup;
};

#endif
