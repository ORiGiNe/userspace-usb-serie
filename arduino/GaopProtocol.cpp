#include "GaopProtocol.h"

Gaop::Gaop()
{
	prochain = 0;
	appel = 0;
	blocked = false;
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
	int ind_buf = prochain++, taille, i;

	unsigned long timeout = micros(); 
	while (ind_buf != appel || blocked) 
	{ 
		if (timeout - micros() >= TIMEOUTSEC*1000000+TIMEOUTUSEC) { appel ++; return false;}
		delayMicroseconds(50); 
	} //tant que ce n'est pas notre tour
	
	octet buf[TAILLE_MAX_FRAME]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	ind_buf = 1; //0 : taille de la frame
	octet checksum = 0; //XOR SUM
	buf[ind_buf++] = cmd.getNbCommandes();
	checksum ^= cmd.getNbCommandes();
	buf[ind_buf++] = odid; 
	checksum ^= odid;
	for (i = 0; i < cmd.getNbCommandes(); i++)
	{
		buf[ind_buf++] = cmd.getTaille(i);
		checksum ^= cmd.getTaille(i);
		for (taille = 0; taille < cmd.getTaille(i); taille++) 
		{
			buf[ind_buf++] = cmd[i][taille];
			checksum ^= cmd[i][taille];
		}
	}
	buf[ind_buf++] = checksum;
	buf[0] = (octet)ind_buf;

	Serial.write(buf, ind_buf*sizeof(octet));
	appel++; //on a fini. Donc on appel le suivant
	if (odid != 0xFF) blocked = true; //attente de disponiblilite
	return true;
}

//inverse de send
bool Gaop::Receive(AssocPeriphOdid& tblassoc) 
{    
	Commande cmd;
	octet odid;
	static bool blockedfriend = false;
	if (blockedfriend == true) { Send(cmd, 0xFF); blockedfriend = false; } //pret a recevoir
	
	int i; //= prochain++;
	//while (i != appel) { delayMicroseconds(50); }
	int j, nb_donnees, taille;
	if (Serial.available() <= 0) { /*appel++;*/ return false; }

	while((octet)Serial.available() < Serial.peek()); //tant que tous les octets ne sont pas arrives.
	
	blockedfriend = true; //si on recoit, c'est que l'autre est dans un etat bloque
	
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
		if (odid == 0xFF) blocked = false;
		else if (tblassoc.getbyodid(odid) != NULL) tblassoc.getbyodid(odid)->Receive(cmd);
		return true;
	} else
	{
		//appel++; //on a fini. Donc, on appel le suivant
		return false;
	}
}

