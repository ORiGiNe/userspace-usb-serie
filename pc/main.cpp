#include <iostream>
#include "GaopProtocol.h"
#include "Effecteur.h"
#include "Capteur.h"
using namespace std;
#include <unistd.h>
#include <ctime>

int main()
{
	AssocPeriphOdid tblassoc;
	Gaop g("/dev/ttyACM0");
<<<<<<< HEAD
//	Effecteur led1(0);		//declaration de la led ayant l'odid 0
//      bite
//	Capteur uson(1);        //ultrason a l'odid 1
	Effecteur servo1(2);	//un servo ayant l'odid 2 (cap'tain obvious à la rescousse)
//	tblassoc.add(&led1);	//enregistrement doit etre fait pour tous les devices
//	tblassoc.add(&uson);
	tblassoc.add(&servo1);	
	g.initialise(&tblassoc); //initailiation des devices au bon gaop protocol


	while (true)
	{	
		servo1.set(180);
		usleep(3000*1000);
		servo1.set(0); 
		usleep(3000*1000);
	}
/* lire indéfiniement les valeurs de l'ultrason *//*
	while (true)
	Effecteur led1(0);		//declaration de la led ayant l'odid 0
	Capteur uson1(1);        //ultrason a l'odid 1
	tblassoc.add(&led1);	//enregistrement doit etre fait pour tous les devices
	tblassoc.add(&uson1);
	g.initialise(&tblassoc); //initailiation des devices au bon gaop protocol

/* 	TEST ULTRASONS I2C
	int val_neg = 0, inter;
	struct timespec avant, apres; 
	clock_gettime(CLOCK_REALTIME, &avant);

	for (int i = 0; i < 1500; i++)
	{
		inter = uson1.get();
		if (inter < 0) val_neg++;
		cout << inter << endl;
	}
		
	clock_gettime(CLOCK_REALTIME, &apres);
	if (apres.tv_nsec - avant.tv_nsec < 0) //retenue
	{
		cout << "Ecouler : " << apres.tv_sec - avant.tv_sec - 1 << 's' << " " << (avant.tv_nsec - apres.tv_nsec)/1000 << "us" << endl;
	} else
	{
		cout << "Ecouler : " << apres.tv_sec - avant.tv_sec << 's' << " " << (apres.tv_nsec - avant.tv_nsec)/1000 << "us" << endl;
	}
	cout << "valeures negative : " << inter << endl;
*/
/*//	Boucle infini jour/nuit
	while (true)
	{
		led1.set(1);
		usleep(1000*1000);
		led1.set(0); 
		usleep(1000*1000);
	}
*/

/*//allume une led puis s'arrete
	led1.set(1);
	sleep(3);
*/	

/*// rapidite
	time_t nb_sec = time(NULL);
	for (unsigned long i = 0; i < 10000; i++)
	{
		led1.set(1);
		led1.set(0);
	}
	cout << "temps ecoule : " << time(NULL) - nb_sec << " s" << endl;
*/
	return 0;
}

