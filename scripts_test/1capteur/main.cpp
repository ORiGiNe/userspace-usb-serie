#include <iostream>
#include "GaopProtocol.h"
#include "Effecteur.h"
#include "Capteur.h"
#include <ctime>
using namespace std;

#define NB_VALEURS 3000

int main()
{
	AssocPeriphOdid tblassoc;
	Gaop g("/dev/ttyACM0");
	Capteur uson(1);        //ultrason a l'odid 1
	tblassoc.add(&uson);	
	g.initialise(&tblassoc); //initailiation des devices au bon gaop protocol


	int val_neg = 0, inter;
	struct timespec avant, apres; 
	clock_gettime(CLOCK_REALTIME, &avant);

	for (int i = 0; i < NB_VALEUR; i++)
	{
		inter = uson.get();
		if (inter < 0) val_neg++;
		//cout << inter << endl;
	}
		
	clock_gettime(CLOCK_REALTIME, &apres);
	cout << "NB_VALEUR : " << NB_VALEUR << endl;
	if (apres.tv_nsec - avant.tv_nsec < 0) //retenue
	{
		cout << "Ecouler : " << apres.tv_sec - avant.tv_sec - 1 << 's' << " " << (avant.tv_nsec - apres.tv_nsec)/1000 << "us" << endl;
	} else
	{
		cout << "Ecouler : " << apres.tv_sec - avant.tv_sec << 's' << " " << (apres.tv_nsec - avant.tv_nsec)/1000 << "us" << endl;
	}
	cout << "valeurs negatives : " << inter << endl;


	return 0;
}

