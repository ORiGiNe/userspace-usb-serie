#include "ArduinoGaop.h"
#include "Config.h"
#include <WProgram.h>

#define DEBUG_ARD() {digitalWrite(13,HIGH); \
	delay(500); \
	digitalWrite(13,LOW); \
	delay(1000); \
}

#define PRINT_ARD(oki) {     Serial.write(oki); Serial.println("oki"); }
ArduinoGaop::ArduinoGaop() : AbstractGaop()
{

}

ArduinoGaop::~ArduinoGaop()
{

}

void ArduinoGaop::initialise(AssocPeriphOdid *tblassoc)
{
	Serial.begin(115200); //valeur maximal pour communication pc/arduino (http://arduino.cc/en/Serial/Begin)
	while (Serial.available() <= 0) //tant que personne ne nous demmande quoi que ce soit
		delay(250); // wait

	Serial.read(); //T'es la ?
	Serial.write("y"); //Ok, je suis demare
	while (Serial.read() != '?'); //enlever tout les signaux "es tu demaree ?"
	//Serial.read(); //combien de device ?
	Serial.write(tblassoc->getNbDevices()); //j'en ai x

	for (int i = 0; i < tblassoc->getNbDevices(); i++)
	{
		while (Serial.read() != 'i'); //Quel est l'ODID du device numero i

		Serial.write( ((*tblassoc)[i])->getOdid());

		while (Serial.available() <= 0);

		if (Serial.read() == 'x') //inconnu => on le desactive
		{
			tblassoc->rm((*tblassoc)[i]->getOdid());
		}
		else //test le
		{
			if ((*tblassoc)[i]->test())
			{
				Serial.write('y');
				(*tblassoc)[i]->associe(this);
			}
			else
			{
				Serial.write('n');
				tblassoc->rm((*tblassoc)[i]->getOdid()); //ca marche pas => desactiver
			}
		}
	}
}

bool ArduinoGaop::send(Commande& cmd, octet odid)
{
	// Envoi
	octet buf[TAILLE_MAX_FRAME] = {0}; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.

	int taille_trame = create_trame(buf, cmd, odid);
	Serial.write(buf, taille_trame);

	return true;
}

bool ArduinoGaop::receive(AssocPeriphOdid& tblassoc)
{    
	Commande cmd;
	Commande nil;
	octet odid = 0;
	int nb_donnees;
	int j;

	// On attend d'avoir des données disponibles
	if (Serial.available() <= 0) 
		return false;

	// On va récupérer la trame dans un buffer avec la taille max de frame
	octet buf[TAILLE_MAX_FRAME] = {0};

	buf[0] = BEGIN_TRAME;
	
	// On attend le début d'une trame
	do
	{
		/* 
FIXME: On attend seulement la taille minimale d'une frame pour la lire : la méthode available n'a pas tout le temps
le comportement attendu : voir avec des flush/peeks & cie, ou alors lire la taille directement. Cela va être génant plus loin
		 */
		while (Serial.available() <= INFOCPL_DEBUT+1)
			delay(1);
	} while (Serial.read() != BEGIN_TRAME);	

	// On récupère le début de cette trame (surtout la taille)
	for (j = 1; j < INFOCPL_DEBUT-1; j++)
	{
		/*while (Serial.available() <= 0)
			delay(1);*/

		buf[j] = Serial.read();
	}

	// On connait la taille, on peut maintenant tout copier
	while (j < buf[IND_TAILLE]+INFOCPL)
	{
		/*while (Serial.available() <= 0)
			delay(1);*/

		buf[j] = Serial.read();
		j++;
	}

	// On essaye de lire cette trame
	if (read_trame(buf,cmd,odid)) 
	{
		// La donnée est bonne : on envoi un ack
		j = buf[1]; //recupere le numero de sequence
		nb_donnees = create_trame(buf, nil, ODIDACKOK);
		// FIXME sale d'écrire comme ça
		buf[1] = j; //on renvoie l'ack avec le meme numero de sequence 
		Serial.write(buf, nb_donnees);
		
		if (odid==ODIDSPECIAL)
		{
			send(nil,ODIDSPECIAL);
			/*octet buf2[TAILLE_MAX_FRAME] = {0};
			buf2[0] = BEGIN_TRAME;
			buf2[1] = 0;
			buf2[2] = 0;
			buf2[3] = ODIDSPECIAL;
			buf2[4] = create_checksum(buf,6);
			buf2[5] = END_TRAME;	
			while(1)
				Serial.write(buf2,6);*/
		}
		
		// Execution de la commande	
		if (tblassoc.getByODID(odid) != NULL)
			tblassoc.getByODID(odid)->receive(cmd);

		return true;
	}
	else
	{
		// Envoi du non ack
		j = buf[1]; //recupere le numero de sequence
		nb_donnees = create_trame(buf, nil, ODIDACKNOK);
		buf[1] = j; //on renvoie l'ack avec le meme numero de sequence 
		Serial.write(buf, nb_donnees*sizeof(octet));	
		return false;
	}
}
