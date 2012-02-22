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
	
	flags |= GAOPSND;
	octet buf[TAILLE_MAX_FRAME]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	ind_buf = 0;
	octet checksum = 0; //XOR SUM
	buf[ind_buf] = cmd.getTaille()*sizeof(short int) + INFOCPL;
	checksum ^= buf[ind_buf++];
	buf[ind_buf++] = odid / 0x100;
	checksum ^= odid / 0x100;
	buf[ind_buf++] = odid % 0x100;
	checksum ^= odid % 0x100;
	for (i = 0; i < cmd.getTaille(); i++)
	{
		buf[ind_buf] = cmd[i] / 0x100;
		checksum ^= buf[ind_buf++];
		buf[ind_buf] = cmd[i] % 0x100;
		checksum ^= buf[ind_buf++];
	}
	buf[ind_buf++] = checksum;

	Serial.write(buf, ind_buf*sizeof(octet));
	if (odid != ODIDSPECIAL) 
	{
		if (++frames_envoyees >= NB_FRAMES_MAX) flags |= GAOPBLK; //attente de disponiblilite
		appel++; //on a fini. Donc on appel le suivant
	} else
	{
		flags &= ~GAOPSPE;
	}
	flags &= ~GAOPSND;
	return true;
}

//inverse de send
bool Gaop::Receive(AssocPeriphOdid& tblassoc) 
{    
	Commande cmd;
	unsigned short int odid;
	if (flags & GAOPDBK) { Send(cmd, ODIDSPECIAL); flags &= ~GAOPDBK; frames_recues = 0; } //pret a recevoir
	
	int i; //= prochain++;
	//while (i != appel) { delayMicroseconds(50); }
	int nb_donnees;
	if (Serial.available() <= 0) { /*appel++;*/ return false; }

	while((octet)Serial.available() < Serial.peek()); //tant que tous les octets ne sont pas arrives.
	
	nb_donnees = Serial.read(); //taille de la frame
	octet checksum = 0;
	checksum ^= nb_donnees;
	nb_donnees = (nb_donnees - INFOCPL)/sizeof(short int);
	odid = Serial.read()*0x100 + Serial.read();
	checksum ^= (odid / 0x100 ) ^ (odid % 0x100);
	for (i = 0; i < nb_donnees; i++)
	{
		cmd[i] = Serial.read() * 0x100 + Serial.read();
		checksum ^= (cmd[i] / 0x100) ^ (cmd[i] % 0x100);
	}
	
	if (odid != ODIDSPECIAL) { if (++frames_recues >= NB_FRAMES_MAX) flags |= GAOPDBK; } //si on recoit, c'est que l'autre est dans un etat bloque
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

