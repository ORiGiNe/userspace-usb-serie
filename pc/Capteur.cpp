#include "Capteur.h"

Capteur::Capteur(int odid) : Peripherique(odid)
{
	reponse = false;
}

void Capteur::Receive(Commande& c)
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
	
	while (!reponse) //Ok car programmation multi-threade
	{
		clock_gettime(CLOCK_REALTIME, &apres);
		if (apres.tv_nsec - avant.tv_nsec < 0) //retenue
		{
			if (apres.tv_sec - avant.tv_sec -1 >= TIMEOUTSEC && avant.tv_nsec - apres.tv_nsec > TIMEOUTUSEC*1000) 
				return -32768;
		} else
		{
			if (apres.tv_sec - avant.tv_sec >= TIMEOUTSEC && apres.tv_nsec - avant.tv_nsec > TIMEOUTUSEC*1000) 
				return -32768;
		}
	
	}
	return cmd[0];
}
