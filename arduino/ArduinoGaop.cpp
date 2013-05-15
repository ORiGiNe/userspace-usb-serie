#include "ArduinoGaop.h"
#include "Config.h"
#include <Arduino.h>
#include "origine_debug.h"

ArduinoGaop::ArduinoGaop() : AbstractGaop()
{

}

ArduinoGaop::~ArduinoGaop()
{

}

void ArduinoGaop::initialise(AssocPeriphOdid *_tblassoc)
{
	int i, j;
	octet trame[TAILLE_MAX_FRAME] = {0};
	Commande init;

	tblassoc = _tblassoc;

	// Open serial communication
	Serial.begin(115200);

	// Begin transmission (ping)
	i = read_trame_from_serial(trame);
	i = create_trame(trame, get_commande_from_trame(trame), get_odid_from_trame(trame));
	Serial.write(trame, i);

	//ORIGINE_DEBUG_ARD_POISON('r', 3);

	// Number of devices
	i = read_trame_from_serial(trame);

	init[0] = tblassoc->getNbDevices();
	i = create_trame(trame, init, ODIDSPECIAL);
	Serial.write(trame, i);

	// For each device, send it odid
	for (j = 0; j < tblassoc->getNbDevices(); j ++ )
	{
		// Wait master command
		i = read_trame_from_serial(trame);
		// FIXME:vérif

		// Send ODID
		init[0] = ((*tblassoc)[j])->getOdid();
		i = create_trame(trame, init, ODIDSPECIAL);
		Serial.write(trame, i);

		//ORIGINE_DEBUG_ARD_POISON(85, 3); // FIXME:ne passe pas ce test

		// If ODID exists in the master, associate it
		i = read_trame_from_serial(trame);
		init = get_commande_from_trame(trame);

		if (init[0] == INIT_ODID_NOK)
		{
			tblassoc->rm((*tblassoc)[j]->getOdid());
		}
		else
		{
			(*tblassoc)[j]->associe(this);
		}
	}
}

bool ArduinoGaop::send(Commande& cmd, octet odid)
{
	octet buf[TAILLE_MAX_FRAME] = {0};

	int taille_trame = create_trame(buf, cmd, odid);
	Serial.write(buf, taille_trame);

	return true;
}

bool ArduinoGaop::receive()
{
	Commande cmd;
	Commande nil;
	octet trame[TAILLE_MAX_FRAME] = {0};
	octet odid = 0;
	int nb_donnees;
	int j;

	// Get trame, send ack or nack
	nb_donnees = read_trame_from_serial(trame);
	
	// If trame valid
	bool verified_trame = verify_trame(trame);
	if (verified_trame)
	{
		// Get informations
		get_data_from_trame(trame, cmd, odid);

		// Exec command
		if (tblassoc->getByODID(odid) != NULL)
			tblassoc->getByODID(odid)->receive(cmd);
		
		// Ping
		if (odid==ODIDSPECIAL)
		{
			// Pong
			send(nil, ODIDSPECIAL);
		}
	}

	return verified_trame;
}

// FIXME:implementer void serialEvent(Serial myPort) && gestion timeout
int ArduinoGaop::read_trame_from_serial(octet* trame)
{
	int i;
	octet o = 0;

	for(;;)
	{
		if (Serial.available())
		{
			o = Serial.read();
			if (o == BEGIN_TRAME)
				break;
		}
		else
		{
			delayMicroseconds(30);
		}
	}
	
	// Wait in order to get size
	while (Serial.available() < IND_TAILLE)
		delayMicroseconds(30);
	
	// Fill the buffer's begin
	trame[0] = o;
	for (i = 1; i < IND_TAILLE + 1; i++)
		trame[i] = Serial.read();

	// We can fill the rest
	while (Serial.available() < (trame[IND_TAILLE] + INFOCPL - (IND_TAILLE + 1)))
		delayMicroseconds(30);

	for (; i < trame[IND_TAILLE] + INFOCPL; i++)
		trame[i] = Serial.read();

	send_ack(verify_trame(trame), trame[IND_SEQ]);

	return (int)(trame[IND_TAILLE] + INFOCPL);
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


