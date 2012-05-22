#include "PCGaop.h"
#include "AssocPeriphOdid.h"
#include "Effecteur.h"
#include "Capteur.h"
#include "Asserv.h"

#include <iostream>
#include <ctime>
#include <sys/stat.h>		/*open*/
#include <fcntl.h>		/*open*/
#include <signal.h>		/*kill*/
#include <unistd.h>		/*read, write, close, fork*/
#include <termios.h>		/*tcgetattr, cfsetospeed, cfsetispeed, tcsetattr, tcdrain struct termios*/
#include <time.h>		/*clock_gettime (implique -lrt), nanosleep*/
#include <iostream>		/*cerr, cout, endl*/
#include <cstring>		/*strerr*/
#include <cerrno>		/*errno*/

using namespace std;

int main()
{
	AssocPeriphOdid tblassoc;
	//AbstractGaop *g = new GaopFactory("/dev/ttyACM0");  // Pour Arduino UNO
	PCGaop g("/dev/ttyACM1");
	//PCGaop g("/dev/ttyUSB0");

	Effecteur led1(1);		//declaration de la led ayant l'odid 0
	//	Capteur uson(1);        //ultrason a l'odid 1
	//		Effecteur servo1(3);	//un servo ayant l'odid 3 : Violet : 5-6
	//		Effecteur servo2(4);	//un servo ayant l'odid 4 : Orange : 9-10
	tblassoc.add(&led1);	//enregistrement doit etre fait pour tous les devices
	//	tblassoc.add(&uson);
	//	tblassoc.add(&servo1);
	//	tblassoc.add(&servo2);
	g.initialise(&tblassoc); //initailiation des devices au bon gaop protocol
	/*	int device = open("/dev/ttyACM1", O_RDWR | O_NOCTTY | O_NDELAY );
	octet trame[TAILLE_MAX_FRAME] = {0};
	octet trame2[TAILLE_MAX_FRAME] = {0};
	octet checksum = 0;
	int i,j, ret;
	int seq = 0;
	Commande data;

	while(1)
	{
		i=0;
		for (j=0;j < TAILLE_MAX_FRAME ; j++)
		{
			trame[j] = 0;
			trame2[j] = 0;
		}
		trame[i++] = BEGIN_TRAME;
		trame[i++] = seq++;
		trame[i++] = 2*data.getTaille(); //FIXME
		trame[i++] = 0;

		for (j = 0; j < data.getTaille(); j++)
		{
			trame[i++] = data[j] / 0x100; //car data[j] = 16 bits
			trame[i++] = data[j] % 0x100;
		}

		checksum = 0;
		for (int j = 0; j < trame[2]+INFOCPL; j++)
			checksum ^= trame[j];

		trame[i+1] = END_TRAME;

		write(device, trame, sizeof(octet)*(INFOCPL+trame[2]));
		sleep(2);
		ret = read(device, trame2, sizeof(octet)*TAILLE_MAX_FRAME);

		cout << "input, attend : " << ret << " | :";
		for (j=0;j < TAILLE_MAX_FRAME ; j++)
			cout << (int)trame[j] << "-";
		cout << endl;
		cout << "output :";
		for (j=0;j < TAILLE_MAX_FRAME ; j++)
			cout << (int)trame2[j] << "-";
		cout << endl;
	}
*/
	/*for( int i = 90 ; i > 0 ; i -- )
		{
			cout << "Servo 1 - Init - : " << i << " : " << servo1.set(i) << endl;
			cout << "Servo 2 - Init - : " << i << " : " << servo2.set(180-i) << endl;
			usleep(100000);
		}*/

	/*
		servo2.set(10);
=======

		// Position Fermée
		servo2.set(20);
>>>>>>> 576c8cbcf3d6ae6dea34aea7bf0aa603ba5144f7
		sleep(1);
		servo1.set(180);
		sleep(1);
		servo3.set(25);
		sleep(1);
		servo4.set(180);
		sleep(4);

		// Position Semi-Ouverte
		servo1.set(90);
		sleep(1);
		servo2.set(110);
		sleep(1);
		servo4.set(90);
		sleep(1);
		servo3.set(110);
		sleep(1);

		// Position Ouverte
		servo1.set(20);
		sleep(1);
		servo2.set(180);
		sleep(1);
		servo4.set(20);
		sleep(1);
		servo3.set(180);
		sleep(1);


		sleep(3600);

<<<<<<< HEAD
		while (true);
*/


	/*sleep(3);
		//cout << "Servo 2 - Init : " << servo2.set(0) << endl;
		sleep(5);
		cout << "Servo 1 - Semi-ouverture : " << servo1.set(90) << endl;
		//sleep(3);
		//cout << "Servo 2 - Semi-ouverture : " << servo2.set(90) << endl;
		sleep(5);
		cout << "Servo 1 - Bloc : " << servo1.set(0) << endl;
		//sleep(3);
		//cout << "Servo 2 - Bloc : " << servo2.set(180) << endl;
		sleep(5);
		cout << "End" << endl;
*/




	/* lire indéfiniement les valeurs de l'ultrason */
	//	Boucle infini jour/nuit
	while (true)
	{
		led1.set(1);
		usleep(100*1000);
		led1.set(0);
		usleep(100*1000);
	}


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

