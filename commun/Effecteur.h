#ifndef DRIVEREFFECTEUR
#define DRIVEREFFECTEUR

#include "Peripherique.h"

class Effecteur : public Peripherique
{
	public:
		Effecteur(int odid, int valInf = -32768, int valSup = 32768);
#if IAmNotOnThePandaBoard
		~Effecteur();
#else
		virtual ~Effecteur();
#endif
		virtual bool set(int valeur) = 0;
		void Receive(Commande&);
	protected:
		int valInf;
		int valSup;
};

#endif
