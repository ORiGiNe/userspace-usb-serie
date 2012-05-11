#include "GaopProtocol.h"

Gaop::Gaop()
{
	prochain = 0;
	appel = 0;
	flags = 0;
	frames_recues = 0;
	frames_envoyees = 0;

}

Gaop::~Gaop()
{

}

void Gaop::initialise(AssocPeriphOdid *tblassoc)
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
	Serial.write(tblassoc->getNbDevices()); //j'en ai x
	for (int i = 0; i < tblassoc->getNbDevices(); i++)
	{
		while (Serial.read() != 'i'); //Quel est l'ODID du device numero i
		Serial.write( ((*tblassoc)[i])->getOdid());
		while (Serial.available() <= 0);
		if (Serial.read() == 'x') //inconnu => on le desactive
		{
			tblassoc->rm((*tblassoc)[i]->getOdid());
		} else //test le
		{
			if ((*tblassoc)[i]->test())
			{
				Serial.write('y');
				(*tblassoc)[i]->associe(this);
			} else
			{
				Serial.write('n');
				tblassoc->rm((*tblassoc)[i]->getOdid()); //ca marche pas => desactiver
			}
		}
	}
}

bool Gaop::Send(Commande& cmd, unsigned short int odid)
{
	int ind_buf, i;

	if (odid != ODIDSPECIAL)
	{
		ind_buf = prochain++;
		unsigned long timeout = micros(); 
		while (ind_buf != appel || flags & (GAOPBLK | GAOPSPE)  ) 
		{ 
			if (micros() - timeout >= TIMEOUTSEC*1000000+TIMEOUTUSEC) { appel ++; return false;}
			delayMicroseconds(50); 
		} //tant que ce n'est pas notre tour
	} else 
	{
		flags |= GAOPSPE;
		while (flags & GAOPSND) delayMicroseconds(50); //il y a deja qqn
	}
	
	flags |= GAOPSND; //dire qu'il y a qqn qui emet
	octet buf[TAILLE_MAX_FRAME]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	int ind_nb_donnee;
	ind_buf = 5; //debut des donnees 
	octet checksum = 0; //XOR SUM
	buf[0] = DEBUT;
	buf[1] = appel;
	checksum ^= buf[1];
	buf[2] = (cmd.getTaille()*sizeof(short int));
	checksum ^= buf[2];
	//buf[3] is checksum
	buf[4] = odid; 
	checksum ^= odid;

	//donnes
	for (ind_nb_donnee = 0; ind_nb_donnee < cmd.getTaille(); ind_nb_donnee++)
	{
		buf[ind_buf] = cmd[ind_nb_donnee] / 0x100;
		checksum ^= buf[ind_buf++];
		buf[ind_buf] = cmd[ind_nb_donnee] % 0x100; //short int -> char
		checksum ^= buf[ind_buf++];
	}
	buf[3] = checksum;
	buf[ind_buf++] = FIN;

	Serial.write(buf, ind_buf*sizeof(octet));
	//tcdrain(device); //attendre que c'est bien envoye
	
	//l'apres devient l'avant
	if (odid != ODIDSPECIAL) 
	{
		if (++frames_envoyees >= NB_FRAMES_MAX) flags |= GAOPBLK; //on attend que la fonction distante se libere a nouveau pour reemettre
		appel++; //appel le suivant
	} else 
	{
		flags &= ~GAOPSPE; //fin de la requete de debloquage
	}
	flags &= ~GAOPSND; //fin de l'emmission
	return true;
}

//inverse de send
bool Gaop::Receive(AssocPeriphOdid& tblassoc) 
{    
	Commande cmd;
	if (flags & GAOPDBK) { Send(cmd, ODIDSPECIAL); flags &= ~GAOPDBK; frames_recues = 0; } //pret a recevoir
	
	int i; //= prochain++;
	//while (i != appel) { delayMicroseconds(50); }
	
	while (Serial.peek() != DEBUT && Serial.available() > 0) Serial.read(); //syncronisation sur le debut
	if (Serial.available() <= 0) return false;
	
	octet num_seq = Serial.read();
	octet nb_donnees = Serial.read();
	octet checksum_recu = Serial.read();
	octet odid = Serial.read();

	if (nb_donnees > 127) return false;
	//verif octet de fin a sa place

	octet checksum = 0;
	checksum ^= nb_donnees ^ num_seq ^ nb_donnees ^ odid;
	for (i = 0; i < nb_donnees; i++)
	{
		cmd[i] = Serial.read() * 0x100 + Serial.read();
		checksum ^= (cmd[i] / 0x100) ^ (cmd[i] % 0x100);
	}
	

	if (odid != ODIDSPECIAL) { if (++frames_recues >= NB_FRAMES_MAX/2) flags |= GAOPDBK; } //si on recoit, c'est que l'autre est dans un etat bloque
	if( Serial.read() != FIN) return false;
	if (checksum == Serial.read())
	{
		//appel++; //on a fini. Donc, on appel le suivant
		if (odid == ODIDSPECIAL) { flags &= ~GAOPBLK; frames_envoyees = 0;} //a recut une frame de debloquage
		else if (tblassoc.getbyodid(odid) != NULL) tblassoc.getbyodid(odid)->Receive(cmd);
		return true;
	} else
	{
		//appel++; //on a fini. Donc, on appel le suivant
		return false;
	}
}

