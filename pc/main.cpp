#include <iostream>
#include "Led.h"
#include "GaopProtocol.h"
using namespace std;
#include <unistd.h>
#include <ctime>

int main()
{
	Led *leds[2];
	Gaop g("/dev/ttyACM0");
	leds[0] = new Led(6);
	leds[1] = new Led(9);
	short int tblodid[2] = {6, 9};
	//initailiation des device au bon gaop protocol
	//les 3 lignes qui suivent devront etre refaites pour tout les slaves
	g.initialise(tblodid, 2);
	if (tblodid[0] >= 0) leds[0]->associe(g);
	if (tblodid[1] >= 0) leds[1]->associe(g);


/*	Boucle infini jour/nuit */
	while (true)
	{
		leds[0]->allume();
		leds[1]->eteint();
		usleep(1000*1000);
		leds[0]->eteint(); 
		leds[1]->allume();
		usleep(1000*1000);
	}

/* allume une led puis s'arrete */
/*
	leds[0]->allume();
	sleep(3);
*/
	/* rapidite*/
/*	time_t nb_sec = time(NULL);
	for (unsigned long i = 0; i < 1000; i++)
	{
		leds[0]->allume();
		leds[1]->allume();
		leds[0]->eteint();
		leds[1]->eteint();
	}
	cout << "temps ecoule : " << time(NULL) - nb_sec << " s" << endl;
*/	
	delete leds[0];
	delete leds[1];
	return 0;
}

