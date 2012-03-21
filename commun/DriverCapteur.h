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
		virtual ~DriverCapteur();
#endif
		virtual int get(); //= 0;
		void Receive(Commande&);
	private:
		bool reponse; //vrai si il y a eu une reponse a la requete
};
#endif
