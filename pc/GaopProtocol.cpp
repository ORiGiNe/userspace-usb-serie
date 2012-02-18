#include "GaopProtocol.h"

Gaop::Gaop(const char *peripherique) 
{
	prochain = 0;
	appel = 0;
	fils = 0;
	flags = 0;	
	pthreadarg = new void*[2];

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
	int x = 0;
	octet odid;
	while (r[0] == 0 && x < 100) //attente d'une connexion
	{
		write(device, ">", 1);	//Je suis demare. Tu es pret ?
		while( read(device, r, 1) == 0);		//Oui
		x++;
	} 
	//	if (x > 100) throw "connection timed out";
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

bool Gaop::Send(Commande &cmd, int odid)
{
	/*Si odid == 0xFF, on envoie une trame special de debloquage. Cette trame ne
	 * doit pas faire la queue et doit passer en mode prioritaire. Elle suit
	 * donc une autre procedure pour prendre un "ticket" */
	int ind_buf;
	struct timespec apres, towait; 

	if (odid != 0xFF)
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
	int ind_taille_donnee, ind_nb_donnee;
	ind_buf = 1; //l'indice 0 contiendra la  taille de la frame
	octet checksum = 0; //XOR SUM
	buf[ind_buf++] = cmd.getNbCommandes();
	checksum ^= cmd.getNbCommandes();
	buf[ind_buf++] = odid % 0x100; 
	checksum ^= odid % 0x100;
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
	buf[0] = ind_buf;

	ind_nb_donnee = write(device, buf, ind_buf*sizeof(octet));
	tcdrain(device); //attendre que c'est bien envoye

	//l'apres devient l'avant
	if (odid != 0xFF) 
	{
		flags |= GAOPBLK; //on attend que la fonction distante se libere a nouveau pour reemettre
		appel++; //appel le suivant
	} else 
	{
		flags &= ~GAOPSPE; //fin de la requete de debloquage
	}
	flags &= ~GAOPSND; //fin de l'emmission
	return (ind_nb_donnee == ind_buf*(int)(sizeof(octet)) && buf[0] == 'y');
}

//inverse de send
bool Gaop::Receive(AssocPeriphOdid& tblassoc) 
{
	Commande cmd;
	if (flags & GAOPDBK) { Send(cmd, 0xFF); flags &= ~GAOPDBK; } //pret a recevoir
	octet odid;
	int ind_buf;// = prochain++;
	//while (ind_buf != appel) { /*usleep(50);*/ } //tant que ce n'est pas notre tour
	octet buf[TAILLE_MAX_FRAME];
	int i, j, nb_donnees, taille;
	ind_buf = 0;

	do	//attendre que les octets arrivent
	{
		if (ind_buf == 0)
		{
			i = read(device, buf, 1); //pour etre sur de lire le bon nombre, ni plus, ni moins
		} else
		{
			i = read(device, buf+ind_buf, (buf[0]-ind_buf)*(sizeof(octet)));
		}
		ind_buf += i;
		if (ind_buf == 0 || i < 0 || ind_buf > TAILLE_MAX_FRAME) { /*appel++;*/ return false; }
	} while (ind_buf != buf[0] && i >= 0);

	ind_buf = 1; //indice[0] = taille de la frame
	octet checksum = 0;
	nb_donnees = buf[ind_buf++];
	checksum ^= nb_donnees;
	odid = buf[ind_buf++];
	checksum ^= odid;

	for (i = 0; i < nb_donnees; i++)
	{
		taille = buf[ind_buf++];
		checksum ^= taille;

		cmd.add(buf+ind_buf, taille);
		ind_buf += taille;
		for (j = 0;  j < taille; j++)
		{			
			checksum ^= cmd[i][j]; //data
		}
	}
	
	if (odid != 0xFF) flags |= GAOPDBK; //si on recoit, c'est que l'autre est dans un etat bloque

	if(buf[ind_buf++] == checksum)
	{
		//appel++;
		if (odid == 0xFF) flags &= ~GAOPBLK; //frame pour dire que l'on peut envoye
		else if (tblassoc.getbyodid(odid) != NULL) tblassoc.getbyodid((int)odid)->Receive(cmd);
		return true;
	} else
	{
		//appel++;
		std::cerr << "Erreur de transmition ! " << std::endl;
		return false;
	}
}

