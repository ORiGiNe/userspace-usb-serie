#include "PCGaop.h"
#include "AssocPeriphOdid.h"

#if DEBUG
using namespace std;
#include <iostream>
#endif

#include <sys/select.h> /* fd_set */
//#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>	/*open*/
#include <fcntl.h>		/*open*/
#include <signal.h>		/*kill*/
#include <unistd.h>		/*read, write, close, fork*/
#include <termios.h>	/*tcgetattr, cfsetospeed, cfsetispeed, tcsetattr, tcdrain struct termios*/
#include <time.h>		/*clock_gettime (implique -lrt), nanosleep*/
#include <iostream>		/*cerr, cout, endl*/
#include <cstring>		/*strerr*/
#include <cerrno>		/*errno*/


PCGaop::PCGaop(const char *slave) : AbstractGaop()
{
	int i;
	pthreadarg = new void*[2];

	this->slave = open(slave, O_RDWR | O_NOCTTY);

	// Mauvais fichier spécial : arrêt
	if (this->slave < 0)
	{
		std::cerr << "Erreur à l'ouverture du fichier spécial : " << strerror(errno) << std::endl;
		throw strerror(errno);
	}

	// Définition des attributs du terminal
	struct termios options;

	tcgetattr(this->slave, &options);

	// Bauds
	cfsetospeed(&options, B115200);
	cfsetispeed(&options, B115200);

	// Options extract from ssty on special device while the analog reader of the Arduino IDE is running
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~PARODD;
	options.c_cflag |= CS8;
	options.c_cflag &= ~HUPCL;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= CREAD | CLOCAL;
	options.c_cflag &= ~CRTSCTS;
	options.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | ISTRIP
			| INLCR | IGNCR | ICRNL | IXON | IXOFF
			| IUCLC | IXANY | IMAXBEL | IUTF8);
	options.c_iflag |= INPCK;
	options.c_oflag &= ~(OPOST | OLCUC | OCRNL | ONLCR | ONOCR
			| ONLRET | OFILL | OFDEL);
	options.c_lflag &= ~(ECHO | ECHONL | ECHOE | ECHOK | ICANON
			| ISIG | IEXTEN | NOFLSH | XCASE | TOSTOP
			| ECHOPRT | ECHOCTL | ECHOKE);
	options.c_oflag |= ( NL0 | CR0 | TAB0 | BS0 | VT0 | FF0 );


	tcsetattr(this->slave, TCSANOW, &options);

	//	tcflush(this->slave, TCIOFLUSH);

	// Initialisation de la structure trame, juste le ack
	trame_envoyees = new Trame[NBR_TRAME_HIST];

	for ( i = 0 ; i < NBR_TRAME_HIST ; i++ )
	{
		trame_envoyees[i] = new trame;
		trame_envoyees[i]->ack = false;
	}

	trames_envoyees = 0;
	periph_busy = false;

	struct timespec now = {2, 0}; 
	nanosleep(&now, NULL);
}

PCGaop::~PCGaop()
{
	if (fils)
		pthread_kill(fils, SIGUSR1);

	if (slave >= 0)
		close(slave);

	delete[] pthreadarg;
}

/*!
 * Thread qui appelle en boucle la fonction receive du GAOP
 * avec son tableau d'ODID.
 * Ce thread est tué par le destructeur du GAOP
 */
void* run_gaop(void* arg)
{
	struct timespec t = {0, 100000}; //100 microsecondes
	void **argv = (void**)arg;

	while (true)
	{
		((PCGaop*)(argv[0]))->
			receive( *((AssocPeriphOdid*)(argv[1])) );
		nanosleep(&t, NULL);
	}

	return NULL;
}

//FIXME:gestion erreurs
void PCGaop::initialise(AssocPeriphOdid *tblassoc)
{
	octet trame[TAILLE_MAX_FRAME] =  {0};
	octet odid = 0;
	int i, size = 0;
	Commande init;

	// FIXME : OUT
	int nb_devices = 0;
	int r[1] = {0};

	// Wait a connection (ie ping)
	init[0] = INIT_READY;
	size = create_trame(trame, init, ODIDSPECIAL);

	int j = write(slave,trame,size);
#ifdef DEBUG
	int h;
	cout << "Trame ( " << j << " ) :";
	for (h=0 ; h < 8 ; h++) //we are waiting for 8 datas
		cout << (int)trame[h] << "-";
	cout	<< endl;
#endif

	i = read_trame_from_fd(trame, init, odid);
#ifdef DEBUG
	cout << "PCGaop::initialise : première trame => " << i << endl;
#endif
	//TODO:error

	// How many devices ?
	init[0] = INIT_NB_DEVICES;
	size = create_trame(trame, init, ODIDSPECIAL);
	write(slave,trame,size);

	i = read_trame_from_fd(trame, init, odid);

	nb_devices = init[0];

#ifdef DEBUG
	cout << "DEBUG Gaop::initialise : Nombre de devices : " << nb_devices << endl;
#endif

	for (int i = 0; i < nb_devices; i++)
	{
		// Get ODID for each device
		init[0] = INIT_GET_ODID;
		size = create_trame(trame, init, ODIDSPECIAL);
		write(slave,trame,size);

		i = read_trame_from_fd(trame, init, odid);

		odid = init[0];

		// Check if this odid is associate with this Gaop
		if (tblassoc->getByODID(odid) != NULL) 
		{
			// Can activate this device
			init[0] = INIT_ODID_OK;
			size = create_trame(trame, init, ODIDSPECIAL);
			write(slave,trame,size);

#if DEBUG
			cout << "DEBUG Gaop::initialise : odid :" << (int)(tblassoc->getByODID(odid)->getOdid()) << " ajouté";
#endif
			tblassoc->getByODID(odid)->associe(this);
		}
		else
		{
			// Otherwise, disable it
			init[0] = INIT_ODID_NOK;
			size = create_trame(trame, init, ODIDSPECIAL);
			write(slave,trame,size);
		}
	}

	// On thread la fonction receive
	pthreadarg[0] = this;
	pthreadarg[1] = tblassoc;
	pthread_create(&fils, NULL, run_gaop, pthreadarg);
}

bool PCGaop::send(Commande &cmd, octet odid)
{
	struct timespec towait;
	int tour = 0;

	// Tant que le périphérique est bloqué, on attend	
	while (periph_busy && odid != ODIDSPECIAL)
	{
		/*
#ifdef DEBUG
cout << "DEBUG PCGaop::send : Periph bloqué" << endl;
#endif
*/
		towait.tv_sec = 0;
		towait.tv_nsec = 500000; //500 microsecondes
		nanosleep(&towait, NULL);
		tour++;

		// Si on dépasse un certain nombre de tours, c'est qu'il y a eu un problème avec la trame de déblocage
		// on considère que c'est great
		if (tour > MAX_NBR_TOURS)
			periph_busy = false;	
	}

	// Si il s'agit d'une trame de déblocage, on bloque en attendant la réponse du périphérique
	if (odid == ODIDSPECIAL)
	{
#ifdef DEBUG
		cout << "DEBUG PCGaop::send : ODID Spécial" << endl;
#endif
	}

	while ((flags & GAOPSND))
		nanosleep(&towait, NULL); //qqn emet => attente

	// Emission et construction
	flags |= GAOPSND; 
	octet buf[TAILLE_MAX_FRAME] = {0};

	int taille_trame = create_trame(buf, cmd, odid);

	// On sauvegarde la trame en cas de non ack ou timeout
	save_trame_before_send(buf);

	int octets_envoyes = write(slave, buf, taille_trame*sizeof(octet));
	//tcdrain(slave); //attendre que c'est bien envoye

#if DEBUG 
	cout << "DEBUG PCGaop::send : Nombre octets ( " << sizeof(octet) << " ) envoyés : " << octets_envoyes << endl;
#endif

	flags &= ~GAOPSND; //fin de l'emission

	if (odid != ODIDSPECIAL)
	{
		if (++trames_envoyees > NB_FRAMES_MAX)
		{
			Commande nil;

			periph_busy = true;
			send(nil,ODIDSPECIAL);
		}
	}

	return (octets_envoyes == taille_trame);
}

bool PCGaop::receive(AssocPeriphOdid& tblassoc)
{
	Commande cmd;
	octet odid;
	octet buf[TAILLE_MAX_FRAME] = {0};
	int i = 0, nb_donnees = 0;
	struct timespec towait;

#ifdef DEBUG
	int k;
	cout << "DEBUG PCGaop::receive : init" << endl;
#endif

	// Si on envoi, on ne fait rien
	if (flags & GAOPSND)
		return false;

	// Récupération des données
	do	
	{
		// Détection d'un début d'une trame
		if (nb_donnees == 0)
		{
			// On boucle tant que l'on n'a pas un début de trame
			i = read(slave, buf, 1*sizeof(octet));

			if (buf[0] == BEGIN_TRAME && i > 0)
				nb_donnees++;
		}
		// Récupération de la taille
		else if (nb_donnees < IND_TAILLE+1)
		{ 
			i = read(slave, buf+nb_donnees, (INFOCPL_DEBUT - nb_donnees)*(sizeof(octet)));

			if ( i > 0 )
				nb_donnees += i;
		}
		else //lit le reste = data + fin
		{
			if (nb_donnees < buf[IND_TAILLE]+INFOCPL)
			{
				i = read(slave, buf+nb_donnees, (buf[2]+INFOCPL - nb_donnees)*(sizeof(octet)));

				if ( i > 0 )
					nb_donnees += i;
			}
		}

		/*
		   Traitement erreurs
		   */
		// Taille
		if (nb_donnees >= IND_TAILLE+1)
		{
			if (buf[IND_TAILLE] > TAILLE_MAX_FRAME-INFOCPL)
				break;
		}

		/*
FIXME: Fail de lecture répétées
*/
		// Boucle principal
		towait.tv_sec = 0;
		towait.tv_nsec = 50000; //50 microsecondes
		nanosleep(&towait, NULL);

#ifdef DEBUG
		cout << "DEBUG PCGaop::receive : Nombre de données lues via read : " << nb_donnees-1 << endl;
		cout << "Derniere valeur de i : " << i << endl;

		if (nb_donnees >= 3 )
			cout << "Nombre de données attendues : " << (buf[2] + INFOCPL) <<endl;

		if (i < 0)
			cout << "Erreur : " <<strerror(errno) <<endl;

		cout << "Donnée actuelle : ";

		for(k=0;k< nb_donnees;k++)
			cout << (int)buf[k] << "-";
		cout << endl;
#endif

		// Si on n'a pas la taille, on revient directement au début
		if (nb_donnees < INFOCPL_DEBUT)
			continue;

		// Si la taille est plus grande que la taille max de frame, on break pour gestion d'erreurs
		if (buf[IND_TAILLE] > TAILLE_MAX_FRAME-INFOCPL)
			break;

	} while (nb_donnees < (int)(buf[2]+INFOCPL)); // TODO:cpt error
	// Fin de la récupération des données

	// Tentative de lecture de la trame
	if ( buf[IND_TAILLE] <= TAILLE_MAX_FRAME-INFOCPL && read_trame(buf, cmd, odid) )
	{ 
#if DEBUG
		cout << "DEBUG PCGaop::receive : Succès de lecture de la trame" <<endl;
#endif

		// En cas d'ODID spécial, on débloque
		if (odid == ODIDSPECIAL)
		{
#if DEBUG
			cout << "DEBUG PCGaop::receive : ODID spécial" <<endl;
#endif

			trames_envoyees = 0;
			periph_busy = false;
		}
		// Gestion de l'ack
		else if ( odid == ODIDACKNOK || odid == ODIDACKOK )
		{
			// ack ok
			if (odid == ODIDACKOK)
			{
#ifdef DEBUG
				cout << "DEBUG PCGaop::receive : ack de la cmd " << (int)buf[1] << endl;
#endif
				trame_envoyees[buf[1]]->ack = true;
			}
			// ack non ok => on renvoi la trame
			else
			{
#ifdef DEBUG
				cout << "DEBUG PCGaop::receive : nack" << endl;
#endif
				Commande cmd_ack;

				build_trame_from_seq(cmd_ack, buf[IND_SEQ]);

				send(cmd_ack,odid);
			}
		}
		else if (tblassoc.getByODID(odid) != NULL)
		{
#ifdef DEBUG
			cout << "DEBUG PCGaop::receive : envoi a l'odid " << (int)odid <<endl;
#endif

			tblassoc.getByODID(odid)->receive(cmd);
		}
		return true;
	}
	// Erreur de lecture de trame 
	else
	{
		// TODO
		// On vérifie ici s'il s'agit d'un ack ou non grace à l'historique,
		// si ce n'est pas le cas on renvoi la dernière requête de type get ou asserv

#ifdef DEBUG
		std::cerr << "Erreur de transmission ! " << std::endl;
#endif
		return false;
	}
}

void PCGaop::save_trame_before_send(octet* buf)
{
	Commande cmd;
	octet i;

	// Récupération du numéro de séquence <=> indice du tableau
	int ind = buf[IND_SEQ];

	// On passe l'ack à false
	trame_envoyees[ind]->ack = false;

	// Sauvegarde de la taille et de l'odid
	trame_envoyees[ind]->taille = buf[IND_TAILLE];
	trame_envoyees[ind]->odid = buf[IND_ODID];

	// Sauvegarde de la commande
	for ( i = 0 ; i < trame_envoyees[ind]->taille/2 ; i++ )
		cmd[i] = buf[2*i+INFOCPL_DEBUT]*0x100 + buf[2*i+INFOCPL_DEBUT+1];
}

octet* PCGaop::build_trame_from_seq(Commande& cmd, octet seq)
{
	octet* buf;

	//TODO
	return buf;
}

int PCGaop::read_trame_from_fd(octet* trame,Commande &cmd, octet &odid)
{
	int nfds = slave + 1;
	int ret = -1;
	int i;
	int read_len = -1;
	int trame_len = 0;

	while(trame_len < TAILLE_MAX_FRAME) // FIXME
	{
		// Reset monitoring on fd
		FD_ZERO(&fdr);
		FD_ZERO(&fdw);
		FD_ZERO(&fde);

		FD_SET(slave, &fdr);

		ret = select(nfds, &fdr, &fdw, &fde, NULL);

		// Read disponible
		if (FD_ISSET(slave, &fdr))
		{
			read_len = read(slave, trame+trame_len, TAILLE_MAX_FRAME-trame_len);

			trame_len += read_len;

#ifdef DEBUG
			cout << "DEBUG PCGaop::read_trame_from_fd : Nombre de données lues via read : " << trame_len << endl;
			cout << "Derniere valeur de read_len : " << read_len << endl;

			cout << "Données actuelles : ";

			int k;
			for(k=0; k < /*trame_len*/ TAILLE_MAX_FRAME; k++)
				cout << (int)trame[k] << "-";
			cout << endl;
#endif

			// En cas de décalage, on se recalibre sur un début de trame
			while (trame[0] != BEGIN_TRAME && trame_len != 0)
			{
#ifdef DEBUG
				cout << "DEBUG PCGaop::read_trame_from_fd : Nombre de décalage" << endl;
#endif
				for ( i = 1 ; i < trame_len ; i++ )
					trame[i-1] = trame[i];

				trame_len--;
			}

		}

		if (trame_len > IND_TAILLE && trame_len >= trame[IND_TAILLE]+INFOCPL && read_trame(trame,cmd,odid))
			return trame_len;
	}

	return -1;
}

