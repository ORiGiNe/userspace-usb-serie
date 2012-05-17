#include "Asserv.h"
#include <stdlib.h>
#include <time.h>

Asserv::Asserv(octet odid) : Peripherique(odid)
{
	reponse = false;
}

void Asserv::receive(Commande& c)
{
	cmd = c;
	reponse = true;
}

bool Asserv::avancer(int distance, int vitesse, int acceleration)
{
	cmd[0] = 64; // setNew
	cmd[1] = distance;
	cmd[2] = vitesse;
	cmd[3] = acceleration;
	cmd[4] = 0;
	cmd[5] = 0;
	cmd[6] = 0;

	return operation();
}

bool Asserv::tourner(int angle, int vitesse, int acceleration)
{
	cmd[0] = 64; // setNew
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;
	cmd[4] = angle;
	cmd[5] = vitesse;
	cmd[6] = acceleration;

	return operation();
}

bool Asserv::arreter()
{
	return avancer(0,0,0);
}

Commande Asserv::get()
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
				return NULL;
		} else
		{
			if (apres.tv_sec - avant.tv_sec >= TIMEOUTSEC && apres.tv_nsec - avant.tv_nsec > TIMEOUTUSEC*1000)
				return NULL;
		}

	}
	return cmd;
}

int Asserv::getLastDistance()
{
	Commande trame = this->get();

	if (trame == NULL)
		return -32768;

	return trame[1];
}

int Asserv::getLastRotation()
{
	Commande trame = this->get();

	if (trame == NULL)
		return -32768;

	return trame[3];
}
