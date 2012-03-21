#ifndef DRIVERCAPTEUR
#define DRIVERCAPTEUR

#include "Peripherique.h"

class Capteur : public Peripherique
{
	public:
		Capteur(int odid);
#if IAmNotOnThePandaBoard
		~Capteur();
#else
		virtual ~Capteur();
#endif
		virtual int get(); //= 0;
		void Receive(Commande&);
	private:
		bool reponse; //vrai si il y a eu une reponse a la requete
};
#endif
