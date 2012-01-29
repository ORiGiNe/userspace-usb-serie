#include "GaopProtocol.h"
#include <WProgram.h> //to have HIGH, LOW, digitalWrite, digitalRead, Serial.*, ...

Gaop::Gaop()
{

}

Gaop::~Gaop()
{

}

void Gaop::initialise(short int odidPeripheriques[] , int nombre_de_devices)
{
	Serial.begin(115200); //valeur maximal pour communication pc/arduino (http://arduino.cc/en/Serial/Begin) 
  	while (Serial.available() <= 0) //tant que personne ne nous demmande quoi que ce soit
	{
		digitalWrite(13, HIGH);
		delay(250);
		digitalWrite(13, LOW);
		delay(250); // wait
	}
	Serial.read(); //T'es la ?
	Serial.write("y"); //Ok, je suis demare
    while (Serial.read() != '?'); //enlever tout les signaux "es tu demaree ?"
	//Serial.read(); //combien de device ?
	Serial.write(nombre_de_devices); //j'en ai x
	for (int i = 0; i < nombre_de_devices; i++)
	{
		while (Serial.read() != 'i'); //Quel est l'ODID du device numero i
		Serial.write(odidPeripheriques[i]);
		while (Serial.available() <= 0);
		if (Serial.read() == 'x') //inconnu => on le desactive
		{
			odidPeripheriques[i] = -1;
		} else //test le
		{
			if (odidPeripheriques[i] >= 0)
			{
				Serial.write('y');
			} else
			{
				Serial.write('n');
			}
		}
	}
}

bool Gaop::Send(Commande& cmd, octet odid)
{
	octet buf[BUF_MAX]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	int ind_taille_donnee, ind_nb_donnee, ind_buf = 0;
	octet checksum = 0; //XOR SUM
	//buf[ind_buf++] = DEBUT;
	buf[ind_buf++] = cmd.getNbCommandes();
	checksum ^= cmd.getNbCommandes();
	buf[ind_buf++] = odid; 
	checksum ^= odid;
	for (ind_nb_donnee = 0; ind_nb_donnee < cmd.getNbCommandes(); ind_nb_donnee++)
	{
		buf[ind_buf++] = cmd.getTaille(ind_nb_donnee);
		checksum ^= cmd.getTaille(ind_nb_donnee);
		for (ind_taille_donnee = 0; ind_taille_donnee < cmd.getTaille(ind_nb_donnee); ind_taille_donnee++) 
		{
			buf[ind_buf++] = cmd[ind_nb_donnee][ind_taille_donnee];
			checksum ^= cmd[ind_nb_donnee][ind_taille_donnee];
		}
	}
	buf[ind_buf++] = checksum;
	//buf[ind_buf++] = FIN;

	Serial.write(buf, ind_buf*sizeof(octet));
	return true;
}

//inverse de send
bool Gaop::Receive(Commande &cmd, octet &odid) 
{    
	int i, j, nb_donnees, taille;
	if (Serial.available() <= 0) return false;

	//buf[ind_buf++]; //DEBUT
	octet checksum = 0;
	while(Serial.peek() < 0);
	nb_donnees = Serial.read();
	checksum ^= nb_donnees;
	while(Serial.peek() < 0);	
	odid = Serial.read();
	checksum ^= odid;
	for (i = 0; i < nb_donnees; i++)
	{
		while(Serial.peek() < 0);
		taille = Serial.read();
		checksum ^= taille;
		
		cmd.add(NULL, taille);
		for (j = 0; j < taille; j++)
		{			
			while(Serial.peek() < 0);
			cmd[i][j] = Serial.read();
			checksum ^= cmd[i][j]; //data
		}
	}
 
	while(Serial.peek() < 0);	
	if (checksum == Serial.read())
	{ 
		//buf[ind_buf++]; //FIN
		return true;
	} else
	{
		odid = 0;
		//buf[ind_buf++]; //FIN
		return false;
	}
}

