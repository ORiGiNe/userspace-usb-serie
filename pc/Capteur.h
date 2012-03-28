#ifndef GAOPCAPTEUR
#define GAOPCAPTEUR

#include "Peripherique.h"
#include <time.h>		/*timeout du get*/ 

class Capteur : public Peripherique
{
	public:
		Capteur(int odid);
		int get();  //renvoie -32768 si timeout expirer
		void Receive(Commande&); //traitement de la réponse au get
	private:
		bool reponse; //vrai si il y a eu une reponse a la requete
};

#endif /*GAOPCAPTEUR*/
