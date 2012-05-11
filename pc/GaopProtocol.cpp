#include "GaopProtocol.h"

Gaop::Gaop(const char *peripherique) 
{
	prochain = 0;
	appel = 0;
	fils = 0;
	flags = 0;	
	pthreadarg = new void*[2];
	frames_envoyees = 0; frames_recues = 0;

	device = open(peripherique, O_RDWR | O_NOCTTY | O_SYNC );
	if (device < 0) 
	{
		std::cerr << strerror(errno) << std::endl;
		throw strerror(errno); //on n'essaye pas de poursuivre
	}
	struct timespec now = {2, 0}; //wait 1 sec
	nanosleep(&now, NULL);
	struct termios options;
	tcgetattr(device, &options);
	cfsetospeed(&options, B115200);
	cfsetispeed(&options, B115200);
	tcsetattr(device, TCSANOW, &options);
}

Gaop::~Gaop()
{
	if (fils) pthread_kill(fils, SIGUSR1); //maintenant c'est fini
	if (device >= 0) close(device);
	delete[] pthreadarg;
}

void* run_gaop(void* arg)
{
	struct timespec t = {0, 10000}; //10 microsecondes
	void **argv = (void**)arg;
	while (true)
	{
		((Gaop*)(argv[0]))->
			Receive( *((AssocPeriphOdid*)(argv[1])) );
		nanosleep(&t, NULL);
	} //jusqu'a ce que je recoive un signal stop !
	return NULL;
}

void Gaop::initialise(AssocPeriphOdid *tblassoc)
{
	/* Protocole de communication : 
	 * PC:
	 * '>' : Je suis demare. Tu es pret
	 * '?' : Tu as combien de devices ?
	 * 'i' : donne moi l'odid du device i
	 * 't' : test son fonctionnement
	 * ARDUINO:
	 * 'y' : oui
	 * 'n' : non
	 * <other> :  other data
	 */

	/* demade au slave (aduino) des informations sur ces devices (capteurs / effecteurs) */
	octet r[1] =  {0};
	int x;
	octet odid;
	
	//attente d'une connexion
	write(device, ">", 1);	//Je suis demare. Tu es pret ?
	while( read(device, r, 1) == 0);		//Oui
	
	write(device, "?", 1);	//Tu as combien de peripheriques ?
	while (read(device, r, 1) == 0);		//J'en ai x
	x = r[0];
	for (int i = 0; i < x; i++)
	{
		write(device, "i", 1);	//Donne moi l'ODID du device i si il marche, 0 sinon
		while( read(device, r, 1) == 0);		//tiens, voila
		odid = r[0];

		if (tblassoc->getbyodid(odid) != NULL) //je regarde si je le connait
		{
			write(device, "t", 1); // test son fonctionnement (et desactive le si il ne marche pas)
			while( read(device, r, 1) == 0);
			if (r[0] != 'y') //si ca marche pas
			{
				tblassoc->rm(odid); //suppression par odid
			} else
			{
				//ca marche bien. je dit au peripherique qu'il peut m'utiliser
				tblassoc->getbyodid(odid)->associe(this);
			}
		} else write(device, "x", 1); //je ne le connais pas. desactive le
	}

	//on thread la fonction receive
	pthreadarg[0] = this;
	pthreadarg[1] = tblassoc;
	pthread_create(&fils, NULL, run_gaop, pthreadarg);	
}

bool Gaop::Send(Commande &cmd, unsigned short int odid)
{
	/*Si odid == ODIDSPECIAL, on envoie une trame special de debloquage. Cette trame ne
	 * doit pas faire la queue et doit passer en mode prioritaire. Elle suit
	 * donc une autre procedure pour prendre un "ticket" */
	int ind_buf;
	struct timespec apres, towait; 
	if (odid != ODIDSPECIAL)
	{
		ind_buf	= prochain++;
		clock_gettime(CLOCK_REALTIME, &apres);
		while (ind_buf != appel || flags & (GAOPBLK | GAOPSPE)) //tant que ce n'est pas notre tour ou qu'il y a trop de monde 
		{
			clock_gettime(CLOCK_REALTIME, &towait);
			if (towait.tv_nsec - apres.tv_nsec < 0) //retenue
			{
				if (towait.tv_sec - apres.tv_sec -1 >= TIMEOUTSEC && apres.tv_nsec - towait.tv_nsec > TIMEOUTUSEC*1000) 
				{
					appel++;
					return false;
				}
			} else
			{
				if (towait.tv_sec - apres.tv_sec >= TIMEOUTSEC && towait.tv_nsec - apres.tv_nsec > TIMEOUTUSEC*1000) 
				{
					appel++;
					return false;
				}
			}
			
			towait.tv_sec = 0;
			towait.tv_nsec = 50000; //50 microsecondes
			nanosleep(&towait, NULL);
		}
	} else //trame special	
	{
		flags |= GAOPSPE; //pour eviter que le mec qui a le numero appel + 1 emmete en meme temps que nous
		towait.tv_sec = 0;
		towait.tv_nsec = 50000; //50 microsecondes
		while (flags & GAOPSND) { nanosleep(&towait, NULL); } //qqn emmet => attente
		
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

	ind_nb_donnee = write(device, buf, ind_buf*sizeof(octet));
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
	return (ind_nb_donnee == ind_buf);
}

//inverse de send
bool Gaop::Receive(AssocPeriphOdid& tblassoc) 
{
	Commande cmd;
	if (flags & GAOPDBK) { Send(cmd, ODIDSPECIAL); flags &= ~GAOPDBK; frames_recues = 0; } //pret a recevoir
	octet odid;
	int ind_buf = 0;
	//while (ind_buf != appel) { /*usleep(50);*/ } //tant que ce n'est pas notre tour
	octet buf[TAILLE_MAX_FRAME];
	int i, nb_donnees;

	do	//attendre que les octets arrivent
	{
		if (ind_buf == 0)
		{
			i = read(device, buf, 1); 
			if (buf[0] != DEBUT || i <= 0) return false;
			ind_buf += 1;
			i = read(device, buf+1, INFOCPL-2); //lecture des entetes
			if (buf[2] > TAILLE_MAX_FRAME - INFOCPL) return false; //erreur de taille : trop grande
		} else
		{
			i = read(device, buf+ind_buf, (buf[2]-ind_buf)*(sizeof(octet)));
		}
		ind_buf += i; //nb d'octet effectivement lu
		if (ind_buf == 0 || i < 0 || ind_buf > TAILLE_MAX_FRAME) { return false; }
	} while (ind_buf != buf[2] + INFOCPL && i >= 0); //buf[2] + INFOCPL : y compris la fin

	//on verifie avent de continuer que l'octet de fin est bien a sa place (err de taille)
	if (buf[2] > 127 || buf[buf[2] + INFOCPL -1] != FIN) return false;

	ind_buf = INFOCPL - 1 - 1;  //taille_total - taille de fin - 1
	octet checksum = 0;
	//nb_donnee est la taille des donnees user, sans les encapsulations
	nb_donnees = buf[2]; //nb_donnees < 128
	checksum ^= nb_donnees;
	odid = buf[4];
	checksum ^= odid / 0x100;

	for (i = 0; i < nb_donnees; i++)
	{
		cmd[i] = buf[ind_buf] * 0x100 + buf[ind_buf + 1];
		ind_buf += 2;
		checksum ^= (cmd[i] / 0x100) ^ (cmd[i] % 0x100);
	}
	
	if (odid != ODIDSPECIAL) {if (++frames_recues >= NB_FRAMES_MAX/2) flags |= GAOPDBK; } //si on recoit, c'est que l'autre est dans un etat bloque

	if(buf[3] == checksum)
	{
		if (odid == ODIDSPECIAL) { flags &= ~GAOPBLK; frames_envoyees = 0; } //frame pour dire que l'on peut envoye
		else if (tblassoc.getbyodid(odid) != NULL) tblassoc.getbyodid(odid)->Receive(cmd);
		return true;
	} else
	{
		std::cerr << "Erreur de transmition ! " << std::endl;
		return false;
	}
}

