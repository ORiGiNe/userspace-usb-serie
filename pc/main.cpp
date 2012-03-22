#include <iostream>
#include "GaopProtocol.h"
#include "Effecteur.h"
using namespace std;
#include <unistd.h>
#include <ctime>

int main()
{
	AssocPeriphOdid tblassoc;
	Gaop g("/dev/ttyACM0");
	Effecteur led1(0);		//declaration de la led ayant l'odid 0
	tblassoc.add(&led1);	//enregistrement doit etre fait pour tout les devices
	
	g.initialise(&tblassoc); //initailiation des devices au bon gaop protocol


/*	Boucle infini jour/nuit */
/*	while (true)
	{
		led1.set(1);
		usleep(1000*1000);
		led1.set(0); 
		usleep(1000*1000);
	}
*/
/* allume une led puis s'arrete */
/*
	led1.set(1);
	sleep(3);
*/	
	/* rapidite*/
	time_t nb_sec = time(NULL);
	for (unsigned long i = 0; i < 10000; i++)
	{
		led1.set(1);
		led1.set(0);
	}
	cout << "temps ecoule : " << time(NULL) - nb_sec << " s" << endl;

	return 0;
}

