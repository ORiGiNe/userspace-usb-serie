#include "Commande.h"

Commande::Commande()
{
	cmd = NULL;
	nb_element = 0;
	taille_des_cmd = NULL;
	taille_de_la_frame = 4; //taille +  nbcommande + odid + checksum = taille minimal de la frame
}

Commande::~Commande()
{
	for (int i = 0; i < nb_element; i++)
	{
		free(cmd[i]);
	}
	if (taille_des_cmd) free(taille_des_cmd);
	if (cmd) free(cmd);
}


void Commande::add(const octet* commande, int taille)
{
	if (taille_de_la_frame+1+taille >= TAILLE_MAX_FRAME) return;
	taille_de_la_frame += 1 + taille;
	/* temps pour les malloc, realloc, ... : de l'ordre de 2*10^-7 seconde pour 
	4 octets de commande (sur machine  a 2,4Ghz). Negligeable par rapport au 
	taux de transfert de l'usb*/
	nb_element++;
	cmd = (octet**)realloc(cmd, nb_element*sizeof(octet*));
	cmd[nb_element-1] = (octet*)malloc(taille*sizeof(octet));
	taille_des_cmd = (int*)realloc(taille_des_cmd, nb_element*sizeof(int));
	if (commande) memcpy(cmd[nb_element - 1], commande, taille);
	taille_des_cmd[nb_element - 1] = taille;
}

void Commande::add(int i)
{
	if (taille_de_la_frame+1+sizeof(short int) >= TAILLE_MAX_FRAME) return;
	taille_de_la_frame += 1 + sizeof(short int);
	nb_element++;
	cmd = (octet**)realloc(cmd, nb_element*sizeof(octet*));
	cmd[nb_element-1] = (octet*)malloc(sizeof(short int));
	taille_des_cmd = (int*)realloc(taille_des_cmd, nb_element*sizeof(int));
	cmd[nb_element-1][0] = (octet)(i >> 8);
	cmd[nb_element-1][1] = i % 0x100;
	taille_des_cmd[nb_element - 1] = sizeof(short int);
}

void Commande::remove()
{
	if (nb_element > 0)
	{
		nb_element--;
		taille_de_la_frame -= 1 +  taille_des_cmd[nb_element];
		free(cmd[nb_element]);
		cmd = (octet**)realloc(cmd, nb_element*sizeof(octet*));
		taille_des_cmd = (int*)realloc(taille_des_cmd, nb_element*sizeof(int));
	}
}

octet* Commande::get(int i)
{
	if (i >= 0 && i < nb_element) return cmd[i];
	else return NULL;
}

short int Commande::getint(int i)
{
	if (i >= 0 && i < nb_element) return (short int)cmd[i][0] * 0x100 + (short int)cmd[i][1];
	else return 0;
}

octet* Commande::operator[](int i)
{
	if (i >= 0 && i < nb_element) return cmd[i];
	else return NULL;
}

int Commande::getTaille(int i) const
{
	if (i >= 0 && i < nb_element) return taille_des_cmd[i];
	else return 0;
}

int Commande::getNbCommandes() const
{
	return nb_element;
}

