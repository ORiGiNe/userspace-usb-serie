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
// pour UNO	Gaop g("/dev/ttyACM0");
// pour Duem ttyUSB0
	Gaop g("/dev/ttyACM0"); 

//	Effecteur led1(0);		//declaration de la led ayant l'odid 0
//      bite
	Capteur usonRouge(1);        //ultrason a l'odid 1
	Capteur usonNoir(2);
	Capteur usonVert(3);
	Capteur usonNormal(4);

	tblassoc.add(&usonRouge);	//enregistrement doit etre fait pour tous les devices
	tblassoc.add(&usonNoir);
	tblassoc.add(&usonVert);
	tblassoc.add(&usonNormal);	

	g.initialise(&tblassoc); //initailiation des devices au bon gaop protocol

	int inter[4];

		
while (true)
{
//	eff.set(180);
	
	inter[0] = usonRouge.get();
	inter[1] = usonNoir.get();
	inter[2] = usonVert.get();
	inter[3] = usonNormal.get();

	cout << "Rouge : " << inter[0] << endl;
	cout << "Noir : " << inter[1] << endl;
	cout << "Vert : " << inter[2] << endl;
	cout << "Normal : " << inter[3] << endl;

	usleep(70);
}

	return 0;
}

