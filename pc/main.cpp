#include <iostream>
#include "Led.h"
#include "GaopProtocol.h"
using namespace std;
#include <unistd.h>
#include <ctime>

int main()
{
	AssocPeriphOdid tblassoc;
	Gaop g("/dev/ttyACM0");
	Led led1(0, 6); tblassoc.add(&led1);
	Led led2(1, 9); tblassoc.add(&led2);
	//initailiation des device au bon gaop protocol
	//la ligne qui suit devra etre refaite pour tous les slaves
	g.initialise(&tblassoc);


/*	Boucle infini jour/nuit */
/*	while (true)
	{
		led1.set(1);
		led2.set(0);
		usleep(1000*1000);
		led1.set(0); 
		led2.set(1);
		usleep(1000*1000);
	}
*/
/* set(1) une led puis s'arrete */
/*
	led1.set(1);
	sleep(3);
*/	
	/* rapidite*/

	time_t nb_sec = time(NULL);
	for (unsigned long i = 0; i < 10000; i++)
	{
		led1.set(1);
		led2.set(1);
		led1.set(0);
		led2.set(0);
	}
	cout << "temps ecoule : " << time(NULL) - nb_sec << " s" << endl;

	return 0;
}

