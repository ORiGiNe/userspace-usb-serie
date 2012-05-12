#include "AbstractGaop.h"

AbstractGaop::AbstractGaop()
{
	this->prochain = 0;
	this->appel = 0;
	this->flags = 0;
	this->frames_recues = 0;
	this->frames_envoyees = 0;
}

octet create_checksum( octet *trame, int taille)
{
	octet checksum = 0;
	for (int i = 0; i < taille; i++)
	{
		checksum ^= trame[i];
	}

	//on a inclut le checksum dans le checksum, or c'est une valeur non definie
	//Donc on le retire
	checksum ^= trame[taille - 2]; //juste avant l'octet de fin
	return checksum;
}

int create_trame(octet *trame, commande &data, octet odid)
{
	static octet seq = 0;
	int i = 0;

	trame[i++] = BEGIN_FRAME;
	trame[i++] = seq++;
	trame[i++] = data.getTaille();
    trame[i++] = odid;
	
	for (int j = 0; j < data.getTaille(); j++)
	{
		trame[i++] = data[j] / 0x100; //car data[j] = 16 bits
		trame[i++] = data[j] % 0x100;
	}

	trame[i+1] = END_FRAME;
	trame[i] = create_checksum(trame, data.getTaille() + 6 ); //6 = taille des donnees d'encapsulation
	return i+2;
}

bool read_trame(octet *trame, int taille, commande &data, octet &odid)
{
	//d'abord, on verfie que tout est correcte
	
	//tailles correctes
	if (taille < 6) return false; //6 = donnee encapsulation
	if (trame[2] + 6 != taille || taille % 2 != 0) return false; //mauvaise taille des donnees 

	//octet debut / fin
	if (trame[0] != BEGIN_FRAME) return false;
	if (trame[taille-1] != END_FRAME) return false;
	
	//checksum
	if (create_checksum(trame, taille) != trame[taille-2]) return false;
	
	//maintenant, on extrait
	odid = trame[3];
	for (int i = 0; i < taille[2]; i+=2)
	{
		data[i] = trame[i+4]*0x100 + trame[i+5];
	}

	return true;
}


