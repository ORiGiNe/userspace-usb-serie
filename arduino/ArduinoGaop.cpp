#include "ArduinoGaop.h"
#include "Config.h"
#include <WProgram.h>
#include "origine_debug.h"

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
	i = create_trame(trame, get_commande_from_trame(trame), get_odid_from_trame(trame));
	Serial.write(trame,i);	

	// Number of devices
	i = read_trame_from_serial(trame);

	init[0] = tblassoc->getNbDevices();
	i = create_trame(trame,init,ODIDSPECIAL);
	Serial.write(trame, i);

	// For each device, send it odid
	for (int i = 0; i < tblassoc->getNbDevices(); i++)
	{
		// Wait master commande
		i = read_trame_from_serial(trame);
		// FIXME:vérif

		// Send ODID
		init[0] = (int)(((*tblassoc)[i])->getOdid()); // FIXME: 206 au lieu de 1 : wtf ?
		i = create_trame(trame, init, ODIDSPECIAL);
		Serial.write(trame, i);

		// If ODID exists in the master, associate it 
		i = read_trame_from_serial(trame);
		init = get_commande_from_trame(trame);

		if (init[0] == INIT_ODID_NOK ) 
		{
			tblassoc->rm((*tblassoc)[i]->getOdid());
		}
		else 
		{
				(*tblassoc)[i]->associe(this);
		}
	}
}

//XXX:bool nécessaire ?
bool ArduinoGaop::send(Commande& cmd, octet odid)
{
	// Envoi
	octet buf[TAILLE_MAX_FRAME] = {0}; 

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
	if (verify_trame(buf)) 
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
	octet o = 0;

	// Syncronize with the begin
	while (o != BEGIN_TRAME)
	{
		while (Serial.available() == 0)
			delayMicroseconds(30);

		o = Serial.read();
	}
	
	// Wait in order to get size
	while (Serial.available() < IND_TAILLE) 
		delayMicroseconds(30);
	
	// Fill the buffer's begin
	trame[0] = o;
	for ( i = 1 ; i < IND_TAILLE+1 ; i++ )
		trame[i] = Serial.read();

	// We can fill the rest
	while (Serial.available() < (trame[IND_TAILLE]+INFOCPL-(IND_TAILLE+1)) )
			delayMicroseconds(30);

	for ( ; i < trame[IND_TAILLE]+INFOCPL ; i++ )
		trame[i] = Serial.read();

	send_ack(verify_trame(trame),trame[IND_SEQ]);

	return (int)(trame[IND_TAILLE]+INFOCPL);
}

int ArduinoGaop::send_ack(bool ok, int seq)
{
	octet buf[INFOCPL];
	int i = 0;

	buf[i++] = BEGIN_TRAME;
	buf[i++] = seq;
	buf[i++] = 0;
	buf[i++] = (ok ? ODIDACKOK : ODIDACKNOK);
	buf[i++] = create_checksum(buf, INFOCPL);
	buf[i++] = END_TRAME;
	
	Serial.write(buf, i);
	
	return i;
}


