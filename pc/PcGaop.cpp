#include "PCGaop.h"
#include "AssocPeriphOdid.h"
#include <origine_debug.h> /* ORIGINE_DEBUG_* */

#include <sys/select.h> /* fd_set */
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

	// Wrong special file : stop
	if (this->slave < 0)
	{
		std::cerr << "Error : " << strerror(errno) << std::endl;
		throw strerror(errno);
	}

	// Set terminal attributes
	struct termios options;

	tcgetattr(this->slave, &options);

	// Bauds
	cfsetospeed(&options, B115200);
	cfsetispeed(&options, B115200);

	// Options extract from ssty on special device while the analog reader of the Arduino IDE is running
	options.c_cflag &= ~(PARENB | PARODD | HUPCL | CSTOPB | CRTSCTS);
	options.c_cflag |= ( CS8 | CREAD | CLOCAL );
	
	options.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | ISTRIP
			| INLCR | IGNCR | ICRNL | IXON | IXOFF
			| IUCLC | IXANY | IMAXBEL | IUTF8);
	options.c_iflag |= INPCK;
	
	options.c_oflag &= ~(OPOST | OLCUC | OCRNL | ONLCR | ONOCR
			| ONLRET | OFILL | OFDEL);
	options.c_oflag |= ( NL0 | CR0 | TAB0 | BS0 | VT0 | FF0 );
	
	options.c_lflag &= ~(ECHO | ECHONL | ECHOE | ECHOK | ICANON
			| ISIG | IEXTEN | NOFLSH | XCASE | TOSTOP
			| ECHOPRT | ECHOCTL | ECHOKE);

	tcsetattr(this->slave, TCSANOW, &options);

	// Trames history initialisation => ack false
	trames_history = new Trame[NBR_TRAME_HIST];

	for ( i = 0 ; i < NBR_TRAME_HIST ; i++ )
	{
		trames_history[i] = new trame;
		trames_history[i]->ack = false;
	}

	trames_envoyees = 0;
	periph_busy = false;
	next_trame = new octet[TAILLE_MAX_FRAME]();
	size_next_trame = 0;

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
	int i,j, size = 0;
	Commande init;

	// Wait a connection (ie ping)
	init[0] = INIT_READY;
	size = create_trame(trame, init, ODIDSPECIAL);

	i = write(slave,trame,size);

	ORIGINE_DEBUG_STDOUT("Trame 1 envoyée | Nombre données envoyées: %d\n", i);
	ORIGINE_DEBUG_STDOUT_ITER(trame,i);
	
	wait_ack(trame[IND_SEQ]);

	i = read_trame_from_fd(trame);
	
	ORIGINE_DEBUG_STDOUT("Trame 1 reçue, nombre de données : %d\n\n\n",i);

	init[0] = tblassoc->getNbDevices();

	// How many devices ?
	init[0] = INIT_NB_DEVICES;
	size = create_trame(trame, init, ODIDSPECIAL);
	i = write(slave,trame,size);
	
	ORIGINE_DEBUG_STDOUT("Trame 2 envoyée | Nombre données envoyées: %d\n", i);
	ORIGINE_DEBUG_STDOUT_ITER(trame,i);
	
	wait_ack(trame[IND_SEQ]);

	i = read_trame_from_fd(trame);

	int nb_devices = get_commande_from_trame(trame)[0];

	ORIGINE_DEBUG_STDOUT("Trame 2 reçue, nombre de devices : %d\n\n\n", nb_devices);
	ORIGINE_DEBUG_STDOUT_ITER(trame,i);
	
	for (j = 0; j < nb_devices; j++)
	{
		// Get ODID for each device
		init[0] = INIT_GET_ODID;
		size = create_trame(trame, init, ODIDSPECIAL);
		write(slave,trame,size);
	
		ORIGINE_DEBUG_STDOUT("Trame pour odid envoyée | Nombre données envoyées: %d | Ordre : %d\n", i, init[0]);
		ORIGINE_DEBUG_STDOUT_ITER(trame,i);
		
		wait_ack(trame[IND_SEQ]);

		i = read_trame_from_fd(trame);

		odid = (octet)(get_commande_from_trame(trame)[0]);
			
		ORIGINE_DEBUG_STDOUT("odid n°%d reçu\n\n\n",odid);

		// Check if this odid is associate with this Gaop
		if (tblassoc->getByODID(odid) != NULL) 
		{
			// Can activate this device
			init[0] = INIT_ODID_OK;
			size = create_trame(trame, init, ODIDSPECIAL);
			write(slave,trame,size);

			ORIGINE_DEBUG_STDOUT("odid n°%d ajouté\n\n\n",odid);
			
			wait_ack(trame[IND_SEQ]);
			
			tblassoc->getByODID(odid)->associe(this);
		}
		else
		{
			// Otherwise, disable it
			init[0] = INIT_ODID_NOK;
			size = create_trame(trame, init, ODIDSPECIAL);
			write(slave,trame,size);
			
			ORIGINE_DEBUG_STDOUT("odid n°%d supprime\n\n\n",odid);
	
			wait_ack(trame[IND_SEQ]);
		}
	}
			
	ORIGINE_DEBUG_STDOUT("Init done\n\n\n");

	// Thread the RX way
	pthreadarg[0] = this;
	pthreadarg[1] = tblassoc;
	pthread_create(&fils, NULL, run_gaop, pthreadarg);
}

// TODO:faire des mutex, c'est mieux
bool PCGaop::send(Commande &cmd, octet odid)
{
	struct timespec towait;
	int tour = 0;

	// GAOP lock until we send/receive a special odid
	while (flags & GAOPBLK && odid != ODIDSPECIAL)
	{
		towait.tv_sec = 0;
		towait.tv_nsec = 500000; //500 microsecondes
		nanosleep(&towait, NULL);
		tour++;

		// Si on dépasse un certain nombre de tours, c'est qu'il y a eu un problème avec la trame de déblocage
		/*if (tour > MAX_NBR_TOURS)
			periph_busy = false;	*/
	}

	// If somebody send data, wait
	while ((flags & GAOPSND));

	// Build & send trame
	flags |= GAOPSND; 
	octet buf[TAILLE_MAX_FRAME] = {0};

	int taille_trame = create_trame(buf, cmd, odid);

	// Save trame in order to re-send it if we have a nack 
	save_trame_before_send(buf);

	int octets_envoyes = write(slave, buf, taille_trame*sizeof(octet));

	ORIGINE_DEBUG_STDOUT("Nombre octets envoyés : %d\n", octets_envoyes);

	// End of send
	flags &= ~GAOPSND;

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
	octet trame[TAILLE_MAX_FRAME] = {0};
	int i = 0, nb_donnees = 0;
	struct timespec towait;

	ORIGINE_DEBUG_STDOUT("init\n");

	// If Gaop send, return
	if (flags & GAOPSND)
		return false;

	// Get trame
	nb_donnees = read_trame_from_fd(trame);

	// If trame is valid 
	if ( verify_trame(trame) )
	{ 
		// If odid spécial, we unlock the periph
		if (odid == ODIDSPECIAL)
		{
			ORIGINE_DEBUG_STDOUT("ODID spécial\n");

			trames_envoyees = 0;
			flags &= ~GAOPBLK;
		}
		// Handle ack
		else if ( odid == ODIDACKNOK || odid == ODIDACKOK )
		{
			// ack ok
			if (odid == ODIDACKOK)
			{
				ORIGINE_DEBUG_STDOUT("ack de la cmd %d\n",trame[IND_SEQ]);
				trames_history[trame[IND_SEQ]]->ack = true;
				
				if (trames_envoyees > 0)
					trames_envoyees--;
			}
			// ack non ok => on renvoi la trame TODO
			else
			{
				ORIGINE_DEBUG_STDOUT("nack de la cmd %d\n",trame[IND_SEQ]);
				
				//trame = build_trame_from_seq(trame[IND_SEQ]);

				//send(get_commande_from_trame(trame),get_odid_from_trame(trame));
			}
		}
		else if (tblassoc.getByODID(odid) != NULL)
		{
			ORIGINE_DEBUG_STDOUT("envoi a l'odid %d\n", odid);

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

		std::cerr << "Erreur de transmission ! " << std::endl;
		
		return false;
	}
}

void PCGaop::save_trame_before_send(octet* buf)
{
	Commande cmd;
	octet i;

	// Sequence number <=> tab index 
	int ind = buf[IND_SEQ];

	// We just send the trame
	trames_history[ind]->ack = false;

	// Save data
	trames_history[ind]->taille = buf[IND_TAILLE];
	trames_history[ind]->odid = buf[IND_ODID];

	for ( i = 0 ; i < trames_history[ind]->taille/2 ; i++ )
		cmd[i] = buf[2*i+INFOCPL_DEBUT]*0x100 + buf[2*i+INFOCPL_DEBUT+1];
}

octet* PCGaop::build_trame_from_seq(octet seq)
{
	octet* buf;

	return buf;
}

//TODO:verify_trame
int PCGaop::read_trame_from_fd(octet* trame)
{
	int nfds = slave + 1;
	int ret = -1;
	int i,j;
	int read_len = -1;
	int trame_len = 0;
	struct timeval wait;

	// Si il y a un début de trame dans next_trame, on l'a copie dans la trame
	if (size_next_trame != 0)
	{
		ORIGINE_DEBUG_STDOUT("Données présentes dans next_trame : %d\n",size_next_trame);
		ORIGINE_DEBUG_STDOUT_ITER(next_trame,TAILLE_MAX_FRAME);

		i = 0;
		for ( j = 0 ; i < size_next_trame ; i++)
		{
			trame[j] = next_trame[i];
			j++;
			trame_len++;
		}
		// En cas de décalage, on se recalibre sur un début de trame
		while (trame[0] != BEGIN_TRAME && trame_len != 0)
		{
			ORIGINE_DEBUG_STDOUT("Décalage (suivant trame_len) :\n");
			ORIGINE_DEBUG_STDOUT_ITER(trame,trame_len);

			trame_len--;
		}

		// Verification of the trame
		if (trame_len > IND_TAILLE && trame_len >= trame[IND_TAILLE]+INFOCPL)
		{
			// On copie la fin de la trame lue, dans le cas où l'on ai lus plusieurs trames
			j = 0;
			for( i = trame[IND_TAILLE]+INFOCPL ; i < trame_len ; i++)
			{
				next_trame[j] = trame[i];
				j++;
			}
			size_next_trame = j;
			trame_len = trame[IND_TAILLE]+INFOCPL;

			ORIGINE_DEBUG_STDOUT("Trame ok, issue de next_trame : %d\n",trame_len);
			ORIGINE_DEBUG_STDOUT_ITER(trame,trame_len);

			return trame_len;
		}
	}

	while(trame_len < TAILLE_MAX_FRAME) // FIXME
	{
		// Reset read monitoring on fd
		FD_ZERO(&fdr);
		FD_SET(slave, &fdr);

		ORIGINE_DEBUG_STDOUT("Attente de news données pour lire\n");
	
		wait.tv_sec = 1;
		wait.tv_usec = 0;

		ret = select(nfds, &fdr, NULL, NULL, &wait);

		// If new data on slave
		if (FD_ISSET(slave, &fdr))
		{
			read_len = read(slave, trame+trame_len, TAILLE_MAX_FRAME-trame_len);
			// TODO:vérif
			trame_len += read_len;

			ORIGINE_DEBUG_STDOUT("Nombre de données lues via read (trame_len) : %d\nDerniere valeur de read_len : %d\n", trame_len, read_len);
			ORIGINE_DEBUG_STDOUT_ITER(trame, TAILLE_MAX_FRAME);

			// En cas de décalage, on se recalibre sur un début de trame
			while (trame[0] != BEGIN_TRAME && trame_len != 0)
			{
				ORIGINE_DEBUG_STDOUT("Décalage (suivant trame_len) :\n");
				ORIGINE_DEBUG_STDOUT_ITER(trame,trame_len);

				trame_len--;
			}
		}

		// Verification of the trame
		if (trame_len > IND_TAILLE && trame_len >= trame[IND_TAILLE]+INFOCPL)
		{
			// On copie la fin de la trame lue, dans le cas où l'on ai lus plusieurs trames
			j = 0;
			for( i = trame[IND_TAILLE]+INFOCPL ; i < trame_len ; i++)
			{
				next_trame[j] = trame[i];
				j++;
			}
			size_next_trame = j;

			ORIGINE_DEBUG_STDOUT("Taille de next_trame : %d\n", j);

			trame_len = trame[IND_TAILLE]+INFOCPL;

			ORIGINE_DEBUG_STDOUT("Trame ok : %d\n",trame_len);
			ORIGINE_DEBUG_STDOUT_ITER(trame,trame_len);

			return trame_len;
		}
	}

	return -1;
}

void PCGaop::wait_ack(octet seq)
{
	int i;
	octet odid = 0;
	Commande cmd;
	octet trame[TAILLE_MAX_FRAME] = {0};

	do
	{
		ORIGINE_DEBUG_STDOUT("Attente ack de la séquence %d\n",seq);
		i = read_trame_from_fd(trame);

		odid = get_odid_from_trame(trame);

		if ( odid == ODIDACKOK )
		{
			// TODO: remplir struct
		}
		else if ( odid == ODIDACKNOK )
		{
			// TODO: renvoyé trame
		}
		else
		{
			// Euh, wait ..?
		}
	} while (odid != ODIDACKOK && odid != ODIDACKNOK);
		
	ORIGINE_DEBUG_STDOUT("Reception ack de la séquence %d\n",seq);
}
