#include "Capteur.h"

#include <ctime>

Capteur::Capteur(octet odid) : Peripherique(odid)
{
	reponse = false;
}

void Capteur::receive(Commande& c)
{
	cmd[0] = c[0];
	reponse = true;
}

int Capteur::get()
{
	reponse = false;
	operation(); //envoie la requete
	//attend la reponse
	struct timespec avant, apres;
	clock_gettime(CLOCK_REALTIME, &avant);
	
	int timeoutMilliSec = TIMEOUTSEC * 1000 + TIMEOUTUSEC / 1000;

	while (!reponse) //Ok car programmation multi-threade
	{
		clock_gettime(CLOCK_REALTIME, &apres);
		timeout = (apres.tv_sec - avant.tv_sec) * 1000 + (apres.tv_nsec - avant.tv_nsec) / 1000000;
		if (timeout > timeoutMilliSec)
			return -1;
	}

	return cmd[0];
}
