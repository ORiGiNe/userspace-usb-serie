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
	int ind_buf, i;

	if (odid != ODIDSPECIAL)
	{
		ind_buf = prochain++;
		unsigned long timeout = micros();

		// Tant que ce n'est pas notre tour
		while (ind_buf != appel || flags & (GAOPBLK | GAOPSPE)  )
		{
			if (micros() - timeout >= TIMEOUTSEC*1000000+TIMEOUTUSEC)
			{
				appel ++;
				return false;
			}
			
			delayMicroseconds(50);
		}
	}
	else
	{
		flags |= GAOPSPE;
		while (flags & GAOPSND)
			delayMicroseconds(50);
	}
	
	// Envoi
	flags |= GAOPSND;
	octet buf[TAILLE_MAX_FRAME]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	
	// Envoi
	int taille_trame = create_trame(buf, cmd, odid);
	Serial.write(buf, taille_trame*sizeof(octet));	

	if (odid != ODIDSPECIAL)
	{
		if (++frames_envoyees >= NB_FRAMES_MAX)
			flags |= GAOPBLK; //attente de disponiblilite
		appel++; //on a fini. Donc on appel le suivant
	}
	else
		flags &= ~GAOPSPE;
	
	flags &= ~GAOPSND;
	return true;
}

bool ArduinoGaop::receive(AssocPeriphOdid& tblassoc)
{    
	Commande cmd;
	Commande cmd_pour_ack;
	octet odid = 0;
	int nb_donnees;
	int j;
	
	// Si l'arduino est bloqué, on l'a débloque en envoyant une trame spéciale de déblocage
	if (flags & GAOPDBK)
	{
		send(cmd, ODIDSPECIAL);
		flags &= ~GAOPDBK;
		frames_recues = 0;
	}
	
	//while (i != appel) { delayMicroseconds(50); }

	// On attend d'avoir des données disponibles
	if (Serial.available() <= 0) 
		return false;

	// On va récupérer la trame dans un buffer avec la taille max de frame
	octet buf[TAILLE_MAX_FRAME];

	buf[0] = BEGIN_TRAME;
	// On attend le début d'une trame
	do
	{
		/* 
			 FIXME: On attend seulement la taille minimale d'une frame pour la lire : la méthode available n'a pas tout le temps
			 le comportement attendu : voir avec des flush/peeks & cie, ou alors lire la taille directement. Cela va être génant plus loin
		 */
		while (Serial.available() <= INFOCPL+1)
			delay(1);
	} while (Serial.read() != buf[0]);	


	// On récupère le début de cette trame (surtout la taille)
	for (j = 1; j < INFOCPL_DEBUT-1; j++)
	{
		/*while (Serial.available() <= 0)
			delay(1);*/

		buf[j] = Serial.read();
	}
	
	// On connait la taille, on peut maintenant tout copier
	while (j < buf[2]+INFOCPL)
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
		nb_donnees = create_trame(buf, cmd_pour_ack, ODIDACKOK);
		// FIXME sale d'écrire comme ça
		buf[1] = j; //on renvoie l'ack avec le meme numero de sequence 

		Serial.write(buf, nb_donnees*sizeof(octet));	

		//traitement	
		if (odid != ODIDSPECIAL)
		{
			if (++frames_recues >= NB_FRAMES_MAX)
				flags |= GAOPDBK;
		} 
		else
		{
			flags &= ~GAOPBLK;
			frames_envoyees = 0;
		}
		if (tblassoc.getByODID(odid) != NULL)
			tblassoc.getByODID(odid)->receive(cmd);
			
		return true;
	}
	else
	{
		//non-reception
		j = buf[1]; //recupere le numero de sequence
		nb_donnees = create_trame(buf, cmd_pour_ack, ODIDACKNOK);
		buf[1] = j; //on renvoie l'ack avec le meme numero de sequence 
		Serial.write(buf, nb_donnees*sizeof(octet));	
		return false;
	}
}
