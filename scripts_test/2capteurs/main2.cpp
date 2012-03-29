#include <iostream>
#include "GaopProtocol.h"
#include "Effecteur.h"
#include "Capteur.h"
using namespace std;
#include <ctime>

int main()
{
	AssocPeriphOdid tblassoc;
	Gaop g("/dev/ttyACM0");
	Capteur uson1(1);        //ultrason a l'odid 1
	Capteur uson2(2);        //ultrason a l'odid 1
	tblassoc.add(&uson2);
	tblassoc.add(&uson1);	
	g.initialise(&tblassoc); //initailiation des devices au bon gaop protocol


/* lire indéfiniement les valeurs de l'ultrason */
	int val_neg = 0, inter;
	struct timespec avant, apres; 
	clock_gettime(CLOCK_REALTIME, &avant);

	for (int i = 0; i < 1500; i++)
	{
		inter = uson1.get();
		if (inter < 0) val_neg++;
		inter = uson2.get();
		if (inter < 0) val_neg++;
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

	return 0;
}

