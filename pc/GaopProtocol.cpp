#include "GaopProtocol.h"

Gaop::Gaop(const char *peripherique) 
{
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
}

void Gaop::initialise(short int odidPeripherique[] , int taille)
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
	bool isknow;
	//int odid;
	while (r[0] == 0 && x < 100) //attente d'une connexion
	{
		write(device, ">", 1);	//Je suis demare. Tu es pret ?
		while( read(device, r, 1) == 0);		//Oui
		x++;
	} 
	if (x > 100) throw "connection timed out";
	write(device, "?", 1);	//Tu as combien de peripheriques ?
	while (read(device, r, 1) == 0);		//J'en ai x
	x = r[0];
	for (int i = 0; i < x; i++)
	{
		write(device, "i", 1);	//Donne moi l'ODID du device i si il marche, 0 sinon
		while( read(device, r, 1) == 0);		//tiens, voila
		isknow = false;
		for (int j = 0; j < taille; j++) //je regarde si je le connait
		{
			if (odidPeripherique[j] == r[0])
			{
				isknow = true;
				write(device, "t", 1); // test son fonctionnement (et desactive le si il ne marche pas)
				while( read(device, r, 1) == 0);
				if (r[0] != 'y') //si ca marche pas
				{
					odidPeripherique[j] = -1;
				}
				break;
			} 
		} 
		if (!isknow) write(device, "x", 1); //je ne le connais pas. desactive le
	}
}

bool Gaop::Send(Commande &cmd , octet odid)
{
	octet buf[BUF_MAX]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.
	int ind_taille_donnee, ind_nb_donnee, ind_buf = 0;
	octet checksum = 0; //XOR SUM
	//buf[ind_buf++] = DEBUT;
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
	//buf[ind_buf++] = FIN;
	if (write(device, buf, ind_buf*sizeof(octet)) < 0) return false;
	return true;
}

//inverse de send
bool Gaop::Receive(Commande &cmd, octet &odid) 
{
	octet buf[BUF_MAX];
	int i, j, nb_donnees, taille, ind_buf = 0;
	while (read(device, buf, BUF_MAX*sizeof(octet)) <= 0); //attendre que les octets arrivent
	
	//buf[ind_buf++]; //DEBUT
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
		//buf[ind_buf++]; //FIN
		return true;
	} else
	{
		odid = 0;
		//buf[ind_buf++]; //FIN
		return false;
	}
}

