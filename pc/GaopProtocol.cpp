#include "GaopProtocol.h"

Gaop::Gaop(const char *peripherique) 
{
	prochain = 0;
	appel = 0;
	stop_envoie = false;
	pid_fils = -1;
	
	device = open(peripherique, O_RDWR | O_NOCTTY | O_SYNC );
	if (device < 0) 
	{
		std::cerr << strerror(errno) << std::endl;
		throw strerror(errno);
	}
	usleep(2000*1000); //wait 2000ms
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
			while (true)
		   	{
				Receive(tblassoc);
				//usleep(1*1000);
			} //jusqu'a ce que je recoive un signal stop !
		}
	}
}

bool Gaop::Send(Commande &cmd , octet odid)
{
	int ind_buf = prochain++;
	while (ind_buf != appel || stop_envoie) { usleep(50); } //tant que ce n'est pas notre tour ou qu'il y a trop de monde
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
	//temps d'envoi en nanoseconde = nbroctet*sizeof(octe)*nbr_de_bit_par_octet*10^6(car res en ns)/nbr_de_symboles_maximum_par_seconde'
	usleep((1.5*ind_buf*sizeof(octet)*8*1000000)/115200); //pour que l'arduino est le temps de digerer
	//while (read(device, buf, 1) <= 0) {/*mettre un delay eventuellement*/ } //lire l'accuse de reception
	appel++; //appel le suivant
	//std::cerr  << 'x' << ' ' << buf[0] << std::endl;
	return (ind_nb_donnee == ind_buf*(int)(sizeof(octet)) && buf[0] == 'y');
}

//inverse de send
bool Gaop::Receive(AssocPeriphOdid& tblassoc) 
{
	Commande cmd;
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
			//std::cout << (int)buf[0]<< ' ' <<   ind_buf << std::endl;
		} else
		{
			i = read(device, buf+ind_buf, (buf[0]-ind_buf)*(sizeof(octet)));
		}
		ind_buf += i;	
		if (ind_buf == 0 || i < 0 || ind_buf > BUF_MAX) { /*appel++;*/ return false; }
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

	if(buf[ind_buf++] == checksum)
	{
		//appel++;
		if (odid == 0XFF) {stop_envoie = !stop_envoie; std::cerr << "STOP !" << std::endl;} //on parle trop vite ou on nous dit de reparir
		else if (tblassoc.getbyodid(odid) != NULL) tblassoc.getbyodid(odid)->Receive(cmd);
		return true;
	} else
	{
		//appel++;
		std::cerr << "Erreur de transmition ! " << std::endl;
		return false;
	}
}

