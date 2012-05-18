#include "PCGaop.h"
#include "AssocPeriphOdid.h"

#if DEBUG
using namespace std;
#include <iostream>
#endif

#include <sys/select.h>
#include <sys/time.h>
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

	this->slave = open(slave, O_RDWR | O_NOCTTY | O_NDELAY );

	// Mauvais fichier spécial : arrêt
	if (this->slave < 0)
	{
		std::cerr << "Erreur à l'ouverture du fichier spécial : " << strerror(errno) << std::endl;
		throw strerror(errno);
	}
	
	// Attente de 1 seconde pour démarrage du slave, afin de régler les options du terminal
	struct timespec now = {2, 0}; 
	nanosleep(&now, NULL);
	
	// Définition des attributs du terminal
	struct termios options;
	
	tcgetattr(this->slave, &options);
	
	// Bauds
	cfsetospeed(&options, B115200);
	cfsetispeed(&options, B115200);
	
	tcsetattr(this->slave, TCSANOW, &options);

	// Initialisation de la structure trame, juste le ack
	trame_envoyees = new Trame[NBR_TRAME_HIST];
	
	for ( i = 0 ; i < NBR_TRAME_HIST ; i++ )
	{
		trame_envoyees[i] = new trame;
		trame_envoyees[i]->ack = false;
	}

	trames_envoyees = 0;
	periph_busy = false;
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

void PCGaop::initialise(AssocPeriphOdid *tblassoc)
{
	octet r[1] =  {0}; // FIXME Remplacer par un pointeur vers un octet, au moins, on sait ce qu'on fait comme ça
	int x = 0; // x correspond au nombre de read sans rien à la base, mais plus à rien maintenant
	octet odid;

	//attente d'une connexion
	write(slave, ">", 1);	//Je suis demarre. Tu es pret ?
	while(read(slave, r, 1) == 0);	//Oui
	
	write(slave, "?", 1);	//Tu as combien de peripheriques ? // FIXME, voir wiki pour voir si c'est le vrai pb
	while (read(slave, r, 1) == 0);		//J'en ai x
	x = r[0];
#ifdef DEBUG
	cout << "DEBUG Gaop::initialise : Nombre de slaves : " << x << endl;
#endif
	for (int i = 0; i < x; i++)
	{
		//Donne moi l'ODID du device i si il marche, 0 sinon
		write(slave, "i", 1);
		//tiens, voila
		while( read(slave, r, 1) == 0);
		odid = r[0]; // FIXME l'identifiant est codé sur 1 octet et non 2 (pour les specs, c'est 2 à ce qu'on m'a dit)

		if (tblassoc->getByODID(odid) != NULL) //je regarde si je le connais
		{
#if DEBUG
			cout << "DEBUG Gaop::initialise : odid :" << (int)(tblassoc->getByODID(odid)->getOdid());
#endif
			write(slave, "t", 1); // test son fonctionnement (et desactive le si il ne marche pas) XXX:magie du saint-esprit ?
			while( read(slave, r, 1) == 0);
			if (r[0] != 'y') //si ca marche pas
			{
#if DEBUG
			cout << " supprimé" <<endl;
#endif
				tblassoc->rm(odid); //suppression par odid
			}
			else
			{
#if DEBUG
			cout << " activé" <<endl;
#endif
				//ca marche bien. je dis au peripherique qu'il peut m'utiliser
				tblassoc->getByODID(odid)->associe(this);
			}
		}
		else
			write(slave, "x", 1); //je ne le connais pas. desactive le
	}

	//on thread la fonction receive
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
