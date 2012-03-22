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
	//struct timespec avant, apres; 
	//clock_gettime(CLOCK_REALTIME, &avant);
//TODO : finir le timeout	
	while (!reponse) //Ok car programmation multi-threade
	{
	
	}
	return cmd[0];
}
