#include "GaopProtocol.h"

Gaop::Gaop(const char *peripherique) 
{
	prochain = 0;
	appel = 0;
	pid_fils = -1;
	blocked = true;	
	blockedfriend = true;

	device = open(peripherique, O_RDWR | O_NOCTTY | O_SYNC );
	if (device < 0) 
	{
		std::cerr << strerror(errno) << std::endl;
		throw strerror(errno);
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
	if (device >= 0) close(device);
	if (pid_fils > 0) kill(pid_fils, SIGUSR1); //maintenant c'est fini
}

void Gaop::initialise(AssocPeriphOdid &tblassoc)
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

		if (tblassoc.getbyodid(odid) != NULL) //je regarde si je le connait
		{
			write(device, "t", 1); // test son fonctionnement (et desactive le si il ne marche pas)
			while( read(device, r, 1) == 0);
			if (r[0] != 'y') //si ca marche pas
			{
				tblassoc.rm(odid); //suppression par odid
			} else
			{
				//ca marche bien. je dit au peripherique qu'il peut m'utiliser
				tblassoc.getbyodid(odid)->associe(this);
			}
		} else write(device, "x", 1); //je ne le connais pas. desactive le
	}

	//on fork la fonction receive !
	if (pid_fils < 0) //si il n'y a pas eu de fork
	{
		pid_fils = fork();
		if (pid_fils == 0) //je suis le fils
		{	
			struct timespec t = {0, 10000}; //10 microsecondes
			while (true)
		   	{
				Receive(tblassoc);
				nanosleep(&t, NULL);
			} //jusqu'a ce que je recoive un signal stop !
		}
	}
}

bool Gaop::Send(Commande &cmd, octet odid)
{
	int ind_buf = prochain++;
	struct timespec apres, towait; 
	clock_gettime(CLOCK_REALTIME, &apres);
	while (ind_buf != appel || blocked) //tant que ce n'est pas notre tour ou qu'il y a trop de monde 
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
	octet buf[BUF_MAX]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	int ind_taille_donnee, ind_nb_donnee;
	ind_buf = 1; //l'indice 0 contiendra la  taille de la frame
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
	buf[0] = ind_buf;

	ind_nb_donnee = write(device, buf, ind_buf*sizeof(octet));
	tcdrain(device); //attendre que c'est bien ete envoye
	
	//l'apres devient l'avant
	appel++; //appel le suivant
	blocked = true;
	return (ind_nb_donnee == ind_buf*(int)(sizeof(octet)) && buf[0] == 'y');
}

//inverse de send
bool Gaop::Receive(AssocPeriphOdid& tblassoc) 
{
	Commande cmd;
	if (blockedfriend == true) { Send(cmd, 0xFF); blockedfriend = false; }//pret a recevoir
	octet odid;
	int ind_buf;// = prochain++;
	//while (ind_buf != appel) { /*usleep(50);*/ } //tant que ce n'est pas notre tour
	octet buf[BUF_MAX];
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
		if (ind_buf == 0 || i < 0 || ind_buf > BUF_MAX) { /*appel++;*/ return false; }
	} while (ind_buf != buf[0] && i >= 0);
	
	blockedfriend = true; //si on recoi, c'est que l'autre est dans un etat bloque

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

	if(buf[ind_buf++] == checksum)
	{
		//appel++;
		if (odid == 0xFF) blocked = false; //frame pour dire que l'on peut envoye
		else if (tblassoc.getbyodid(odid) != NULL) tblassoc.getbyodid(odid)->Receive(cmd);
		return true;
	} else
	{
		//appel++;
		std::cerr << "Erreur de transmition ! " << std::endl;
		return false;
	}
}

