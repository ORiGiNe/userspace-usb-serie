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

	return getLastDistance();
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

	return getLastDistance();
}

bool Asserv::arreter()
{
	return avancer(0,0,0);
}

int Asserv::get(Commande &c)
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

	c = cmd;
	return 1;
}

int Asserv::getLastDistance()
{
	Commande trame;
	if (get(trame) == -1)
		return -32768;

	return trame[1];
}

int Asserv::getLastRotation()
{
	Commande trame;
	if (get(trame) == -1)
		return -32768;

	return trame[3];
}
