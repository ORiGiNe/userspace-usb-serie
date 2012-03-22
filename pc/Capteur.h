#ifndef GAOPCAPTEUR
#define GAOPCAPTEUR

#include "Peripherique.h"

class Capteur : public Peripherique
{
	public:
		Capteur(int odid);
		int get(); 
		void Receive(Commande&);
	private:
		bool reponse; //vrai si il y a eu une reponse a la requete
};

#endif /*GAOPCAPTEUR*/
