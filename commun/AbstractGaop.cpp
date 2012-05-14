#include "AbstractGaop.h"

#if DEBUG && !IAmNotOnThePandaBoard
using namespace std;
#include <iostream>

void debug_affiche_trame(octet *trame, int taille)
{
	for(int i = 0 ; i < taille+INFOCPL ; i++)
		 cout << (int)trame[i] << "-";
	cout << endl;
}
#endif

AbstractGaop::AbstractGaop()
{
	prochain = 0;
	appel = 0;
	flags = 0;
	frames_recues = 0;
	frames_envoyees = 0;
}

octet AbstractGaop::create_checksum( octet *trame, int taille)
{
	// On crée le checksum sur les taille-2 octets
	octet checksum = 0;
	for (int i = 0; i < taille-2; i++)
	{
		checksum ^= trame[i];
	}

	return checksum;
}

int AbstractGaop::create_trame(octet *trame, Commande &data, octet odid)
{
	static octet seq = 0;
	int i = 0;

	trame[i++] = BEGIN_TRAME;
	trame[i++] = seq++;
	trame[i++] = 2*data.getTaille(); //FIXME
  trame[i++] = odid;
	
	for (int j = 0; j < data.getTaille(); j++)
	{
		trame[i++] = data[j] / 0x100; //car data[j] = 16 bits
		trame[i++] = data[j] % 0x100;
	}

	trame[i+1] = END_TRAME;
	trame[i] = create_checksum(trame, 2*data.getTaille() + INFOCPL ); // FIXME
	
	#if DEBUG && !IAmNotOnThePandaBoard
		cout << "DEBUG AbstractGaop::create_trame : Trame crée : (taille : " << i+2 << ")" << endl;
		debug_affiche_trame(trame,trame[2]);
	#endif

	return i+2;
}

bool AbstractGaop::read_trame(octet *trame, Commande &cmd, octet &odid)
{
	// Vérification minimale des données
	// Octet fin
	if (trame[0] != BEGIN_TRAME)
		return false;

	// On extrait la taille de la commande pour pouvoir travailler un minimum
	int taille_cmd = trame[2];
	
	if (trame[taille_cmd+INFOCPL-1] != END_TRAME)
		return false;

	if (taille_cmd > TAILLE_MAX_FRAME - INFOCPL)
		return false;

	// Checksum
	if (create_checksum(trame, taille_cmd+INFOCPL) != trame[taille_cmd+INFOCPL-2])
		return false;
	
	// On peut maintenant extraire les informations
	odid = trame[3];
	for (int i = 0; i < taille_cmd/2; i++)
	{
		cmd[i] = trame[2*i+INFOCPL_DEBUT]*0x100 + trame[2*i+INFOCPL_DEBUT+1];
	}

	#if DEBUG && !IAmNotOnThePandaBoard
		cout << "DEBUG AbstractGaop::read_trame : Trame lue : " << endl;
		debug_affiche_trame(trame, taille_cmd);
	#endif

	return true;
}

