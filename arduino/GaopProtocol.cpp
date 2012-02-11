#include "GaopProtocol.h"

Gaop::Gaop()
{
	prochain = 0;
	appel = 0;
	stop_envoie = false;
}

Gaop::~Gaop()
{

}

void Gaop::initialise(AssocPeriphOdid &tblassoc)
{
	Serial.begin(115200); //valeur maximal pour communication pc/arduino (http://arduino.cc/en/Serial/Begin) 
  	while (Serial.available() <= 0) //tant que personne ne nous demmande quoi que ce soit
	{
		delay(250); // wait
	}
	Serial.read(); //T'es la ?
	Serial.write("y"); //Ok, je suis demare
    while (Serial.read() != '?'); //enlever tout les signaux "es tu demaree ?"
	//Serial.read(); //combien de device ?
	Serial.write(tblassoc.getNbDevices()); //j'en ai x
	for (int i = 0; i < tblassoc.getNbDevices(); i++)
	{
		while (Serial.read() != 'i'); //Quel est l'ODID du device numero i
		Serial.write(tblassoc[i]->getOdid());
		while (Serial.available() <= 0);
		if (Serial.read() == 'x') //inconnu => on le desactive
		{
			tblassoc.rm(tblassoc[i]->getOdid());
		} else //test le
		{
			if (tblassoc[i]->test())
			{
				Serial.write('y');
				tblassoc[i]->associe(this);
			} else
			{
				Serial.write('n');
				tblassoc.rm(tblassoc[i]->getOdid()); //ca marche pas => desactiver
			}
		}
	}
}

bool Gaop::Send(Commande& cmd, octet odid)
{
	int ind_buf = prochain++;
	while (ind_buf != appel) { delayMicroseconds(50); } //tant que ce n'est pas notre tour
	
	octet buf[BUF_MAX]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	int ind_taille_donnee, ind_nb_donnee;
	ind_buf = 1; //0 : taille de la frame
	octet checksum = 0; //XOR SUM
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
	buf[0] = (octet)ind_buf;

	Serial.write(buf, ind_buf*sizeof(octet));
	appel++; //on a fini. Donc on appel le suivant
	return true;
}

//inverse de send
bool Gaop::Receive(AssocPeriphOdid& tblassoc) 
{    
	Commande cmd;
	octet odid;
	if (!stop_envoie && Serial.available() > 48) //la pile se remplit dangeresement. on envoie une frame special pour arreter l'emmission
	{
		Send(cmd, 0xFF); //tu parles trop vite
		stop_envoie = true;
	} else if (stop_envoie && Serial.available() < 2) //c'est vide. On peut repartir
	{
		Send(cmd, 0xFF); //go
		stop_envoie = false;
	}
	
	int i; //= prochain++;
	//while (i != appel) { delayMicroseconds(50); }
	int j, nb_donnees, taille;
	if (Serial.available() <= 0) { /*appel++;*/ return false; }

	while((octet)Serial.available() < Serial.peek()); //tant que tous les octets ne sont pas arrives.
	Serial.read(); //taille de la frame
	octet checksum = 0;
	nb_donnees = Serial.read();
	checksum ^= nb_donnees;
	odid = Serial.read();
	checksum ^= odid;
	for (i = 0; i < nb_donnees; i++)
	{
		taille = Serial.read();
		checksum ^= taille;
		
		cmd.add(NULL, taille);
		for (j = 0; j < taille; j++)
		{			
			cmd[i][j] = Serial.read();
			checksum ^= cmd[i][j]; //data
		}
	}
 		
	if (checksum == Serial.read())
	{
		//appel++; //on a fini. Donc, on appel le suivant
		if (tblassoc.getbyodid(odid) != NULL) tblassoc.getbyodid(odid)->Receive(cmd);
		return true;
	} else
	{
		//appel++; //on a fini. Donc, on appel le suivant
		return false;
	}
}

