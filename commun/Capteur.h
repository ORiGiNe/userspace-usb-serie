#ifndef DRIVERCAPTEUR
#define DRIVERCAPTEUR

#include "Peripherique.h"

class DriverCapteur : public Peripherique
{
	public:
		DriverCapteur(int odid);
#if IAmNotOnThePandaBoard
		~DriverCapteur();
#else
		~DriverCapteur();
#endif
		virtual int get() = 0;
		bool test();
		void Receive(Commande&);
	private:
		bool reponse; //vrai si il y a eu une reponse a la requete
};
#endif
