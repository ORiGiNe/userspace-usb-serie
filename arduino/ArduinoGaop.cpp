#include "ArduinoGaop.h"
#include <WProgram.h>

ArduinoGaop::ArduinoGaop() : AbstractGaop()
{

}

ArduinoGaop::~ArduinoGaop()
{

}

void ArduinoGaop::initialise(AssocPeriphOdid *tblassoc)
{
	Serial.begin(115200); //valeur maximal pour communication pc/arduino (http://arduino.cc/en/Serial/Begin)
	while (Serial.available() <= 0) //tant que personne ne nous demmande quoi que ce soit
		delay(250); // wait
	
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
		}
		else //test le
		{
			if ((*tblassoc)[i]->test())
			{
				Serial.write('y');
				(*tblassoc)[i]->associe(this);
			}
			else
			{
				Serial.write('n');
				tblassoc->rm((*tblassoc)[i]->getOdid()); //ca marche pas => desactiver
			}
		}
	}
}

bool ArduinoGaop::send(Commande& cmd, octet odid)
{
	int ind_buf, i;

	if (odid != ODIDSPECIAL)
	{
		ind_buf = prochain++;
		unsigned long timeout = micros();

		// Tant que ce n'est pas notre tour
		while (ind_buf != appel || flags & (GAOPBLK | GAOPSPE)  )
		{
			if (micros() - timeout >= TIMEOUTSEC*1000000+TIMEOUTUSEC)
			{
				appel ++;
				return false;
			}
			
			delayMicroseconds(50);
		}
	}
	else
	{
		flags |= GAOPSPE;
		while (flags & GAOPSND)
			delayMicroseconds(50);
	}
	
	// Envoi
	flags |= GAOPSND;
	octet buf[TAILLE_MAX_FRAME]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	
	// Envoi
	int taille_trame = create_trame(buf, cmd, odid);
	Serial.write(buf, taille_trame*sizeof(octet));	

	if (odid != ODIDSPECIAL)
	{
		if (++frames_envoyees >= NB_FRAMES_MAX)
			flags |= GAOPBLK; //attente de disponiblilite
		appel++; //on a fini. Donc on appel le suivant
	}
	else
		flags &= ~GAOPSPE;
	
	flags &= ~GAOPSND;
	return true;
}

bool ArduinoGaop::receive(AssocPeriphOdid& tblassoc)
{    
	Commande cmd;
	octet odid;
	if (flags & GAOPDBK)
	{
		send(cmd, ODIDSPECIAL);
		flags &= ~GAOPDBK;
		frames_recues = 0;
	} //pret a recevoir
	
	int i; //= prochain++;
	//while (i != appel) { delayMicroseconds(50); }
	int nb_donnees;
	if (Serial.available() < 6) //pas assez pour faire une trame
	{
		return false;
	}

	octet buf[TAILLE_MAX_FRAME];

	while((octet)Serial.available() < Serial.peek()); //tant que tous les octets ne sont pas arrives.
	
	//recuperation de l'entete
	while ((buf[0] = Serial.read()) != BEGIN_FRAME); //synchronisation entete
	for (int i = 1; i < 4; i++)
	{ //recuperation du reste de l'entete
		buf[i] = Serial.read();
	}
	
	//attendre que les octets arrivent
	while (buf[2] + 2 < Serial.available()) delay(1);	
	
	if (read_trame(buf, nb_donnees, cmd, odid))
	{
		if (odid != ODIDSPECIAL)
		{
			if (++frames_recues >= NB_FRAMES_MAX)
				flags |= GAOPDBK;
		} //a recut une frame de debloquage
		else if (tblassoc.getByODID(odid) != NULL)
			tblassoc.getByODID(odid)->receive(cmd);
			
		return true;
	}
	else
	{
		return false;
	}
}
