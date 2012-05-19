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
	int i;
	octet trame[TAILLE_MAX_FRAME] = {0};
	octet odid = 0;
	Commande init;

	// Open serial communication
	Serial.begin(115200); 

	// Begin transmission (ping)
	i = read_trame_from_serial(trame);	

	// Number of devices
	i = read_trame_from_serial(trame);
	//TODO:ack
	init[0] = tblassoc->getNbDevices();
	i = create_trame(trame,init,ODIDSPECIAL);
	Serial.write(trame, i);

	// For each device, send it odid
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
		
		// Execution de la commande	
		if (tblassoc.getByODID(odid) != NULL)
			tblassoc.getByODID(odid)->receive(cmd);
		
		if (odid==ODIDSPECIAL)
		{
			send(nil,ODIDSPECIAL);
		}
		
		

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

// FIXME:implementer void serialEvent(Serial myPort) && gestion timeout
int ArduinoGaop::read_trame_from_serial(octet* trame)
{
	int i;

	//syncronize whith the begin
	while (Serial.peek() != BEGIN_TRAME)
		Serial.read();
	
	// Wait in order to get size
	while (Serial.available() < IND_TAILLE+1) 
		delayMicroseconds(30);

	
	// Fill the buffer's begin
	for ( i = 0 ; i < IND_TAILLE ; i++ )
		trame[i] = Serial.read();

	// We can fill the rest
	while (Serial.available() < (int)(trame[IND_TAILLE]+INFOCPL) )
			delayMicroseconds(30);

	for ( ; i < trame[IND_TAILLE]+INFOCPL ; i++ )
		trame[i] = Serial.read();

	send_ack( trame[IND_TAILLE]+INFOCPL &&               //size ok
			  trame[i-1] == END_TRAME &&                 //end in the good place
			  create_checksum(trame, i) == trame[i-2],      //checksum ok
			  trame[IND_SEQ]                             //with the good sequence number
			);
	return (int)(trame[IND_TAILLE]+INFOCPL);
}

int ArduinoGaop::send_ack(bool ok, int seq)
{
	octet buf[INFOCPL];
	int i = 0;
	buf[i++] = BEGIN_TRAME;
	buf[i++] = seq;
	buf[i++] = 0; //size of data
	buf[i++] = (ok ? ODIDACKOK : ODIDACKNOK);
	buf[i++] = create_checksum(buf, INFOCPL);
	buf[i++] = END_TRAME;
	Serial.write(buf, i);
	return i;
}


