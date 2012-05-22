#include "AbstractGaop.h"

AbstractGaop::AbstractGaop()
{
	flags = 0;
}

octet AbstractGaop::create_checksum( octet *trame, int taille)
{
	octet checksum = 0;

	for (int i = 0; i < taille-2; i++)
		checksum ^= trame[i];

	return checksum;
}

int AbstractGaop::create_trame(octet *trame, Commande data, octet odid)
{
	static octet seq = 0;
	int i = 0;

	trame[i++] = BEGIN_TRAME;
	trame[i++] = seq++;
	trame[i++] = COMMAND_SIZE*data.getTaille(); //FIXME
	trame[i++] = odid;

	// Spécifique à COMMAND_SIZE
	for (int j = 0; j < data.getTaille(); j++)
	{
		trame[i++] = data[j] / 0x100; //car data[j] = 16 bits
		trame[i++] = data[j] % 0x100;
	}

	trame[i+1] = END_TRAME;
	trame[i] = create_checksum(trame, 2 * data.getTaille() + INFOCPL); // FIXME

	return i+2;
}

bool AbstractGaop::verify_trame(octet *trame)
{
	int taille_cmd = trame[IND_TAILLE];

	if (trame[taille_cmd + INFOCPL - 1] != END_TRAME)
		return false;

	if (taille_cmd > TAILLE_MAX_FRAME - INFOCPL)
		return false;

	// Checksum
	if (create_checksum(trame, taille_cmd + INFOCPL) != trame[taille_cmd + INFOCPL - 2])
		return false;

	return true;
}	

void AbstractGaop::get_data_from_trame(octet *trame, Commande &cmd, octet &odid)
{
	odid = get_odid_from_trame(trame);
	cmd = get_commande_from_trame(trame);	
}

/*
void AbstractGaop::reset_trame(octet *trame)
	{
	int i, size = trame[IND_TAILLE];

	for ( i = 0 ; i < size ; i++ )
	trame[i] = 0;
	}
//*/

octet AbstractGaop::get_odid_from_trame(octet *trame)
{
	return trame[IND_ODID];
}

Commande AbstractGaop::get_commande_from_trame(octet *trame)
{
	Commande cmd;
	int taille_cmd = trame[IND_TAILLE];

	// If no data
	if (taille_cmd == 0)
		return cmd;

	// Spécifique à COMMANDE_SIZE
	for (int i = 0; i < taille_cmd/2; i++)
	{
		cmd[i] = trame[2 * i + INFOCPL_DEBUT] * 0x100 + trame[2 * i + INFOCPL_DEBUT + 1];
	}

	return cmd;
}
