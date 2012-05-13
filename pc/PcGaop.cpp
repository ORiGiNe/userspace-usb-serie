#include "PCGaop.h"
#include "AssocPeriphOdid.h"

#if DEBUG
using namespace std;
#include <iostream>
#endif

#include <sys/stat.h>	/*open*/
#include <fcntl.h>		/*open*/
#include <signal.h>		/*kill*/
#include <unistd.h>		/*read, write, close, fork*/
#include <termios.h>	/*tcgetattr, cfsetospeed, cfsetispeed, tcsetattr, tcdrain struct termios*/
#include <time.h>		/*clock_gettime (implique -lrt), nanosleep*/
#include <iostream>		/*cerr, cout, endl*/
#include <cstring>		/*strerr*/
#include <cerrno>		/*errno*/

/*!
*	Constructeur du protocole GAOP coté PC
*	@param peripherique : Chemin absolu du device special
* - Ouverture du device
* - Set des paramètres liés à la communication série
* - Création de la structure du thread qui bouclera en réception
*/
PCGaop::PCGaop(const char *peripherique) : AbstractGaop()
{
	pthreadarg = new void*[2];

	// TODO: Toucher aux diverses options pour faire fonctionner sur arm & x86
	device = open(peripherique, O_RDWR | O_NOCTTY | O_SYNC );

	// Mauvais device : arrêt
	if (device < 0)
	{
		std::cerr << strerror(errno) << std::endl;
		throw strerror(errno);
	}
	
	// Définition des attributs du terminal
	struct timespec now = {2, 0}; //wait 1 sec
	nanosleep(&now, NULL);
	struct termios options;
	tcgetattr(device, &options);
	cfsetospeed(&options, B115200);
	cfsetispeed(&options, B115200);
	tcsetattr(device, TCSANOW, &options);
}

PCGaop::~PCGaop()
{
	if (fils)
		pthread_kill(fils, SIGUSR1);
	
	if (device >= 0)
		close(device);
	
	delete[] pthreadarg;
}

/*!
 * Thread qui appellera en boucle la fonction receive du GAOP
 * avec son tableau d'ODID.
 * Ce thread est tué par le destructeur du GAOP
 */
void* run_gaop(void* arg)
{
	struct timespec t = {0, 10000}; //10 microsecondes
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
	/* demade au slave (aduino) des informations sur ces devices (capteurs / effecteurs) */
	octet r[1] =  {0}; // FIXME Remplacer par un pointeur vers un octet, au moins, on sait ce qu'on fait comme ça
	int x = 0; // x correspond au nombre de read sans rien à la base, mais plus à rien maintenant
	octet odid;

	//attente d'une connexion
	write(device, ">", 1);	//Je suis demare. Tu es pret ?
	while(read(device, r, 1) == 0);	//Oui
	
	write(device, "?", 1);	//Tu as combien de peripheriques ? // FIXME, voir wiki pour voir si c'est le vrai pb
	while (read(device, r, 1) == 0);		//J'en ai x
	x = r[0];
#ifdef DEBUG
	cout << "DEBUG Gaop::initialise : Nombre de devices : " << x << endl;
#endif
	for (int i = 0; i < x; i++)
	{
		//Donne moi l'ODID du device i si il marche, 0 sinon
		write(device, "i", 1);
		//tiens, voila
		while( read(device, r, 1) == 0);
		odid = r[0]; // FIXME l'identifiant est codé sur 1 octet et non 2 (pour les specs, c'est 2 à ce qu'on m'a dit)

		if (tblassoc->getByODID(odid) != NULL) //je regarde si je le connais
		{
#if DEBUG
			cout << "DEBUG Gaop::initialise : odid :" << (int)(tblassoc->getByODID(odid)->getOdid());
#endif
			write(device, "t", 1); // test son fonctionnement (et desactive le si il ne marche pas) XXX:magie du saint-esprit ?
			while( read(device, r, 1) == 0);
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
			write(device, "x", 1); //je ne le connais pas. desactive le
	}

	//on thread la fonction receive
	pthreadarg[0] = this;
	pthreadarg[1] = tblassoc;
	pthread_create(&fils, NULL, run_gaop, pthreadarg);
}

bool PCGaop::send(Commande &cmd, octet odid)
{
	/*Si odid == ODIDSPECIAL, on envoie une trame special de debloquage. Cette trame ne
	 * doit pas faire la queue et doit passer en mode prioritaire. Elle suit
	 * donc une autre procedure pour prendre un "ticket" */

	int numero; //notre numero dans la file d'attente
	struct timespec apres, towait;
	
	if (odid != ODIDSPECIAL)
	{
		numero = prochain++;
		clock_gettime(CLOCK_REALTIME, &apres);

		while (numero != appel || flags & (GAOPBLK | GAOPSPE)) //tant que ce n'est pas notre tour ou qu'il y a trop de monde
		{
			clock_gettime(CLOCK_REALTIME, &towait);
			if (towait.tv_nsec - apres.tv_nsec < 0) //retenue
			{
				if (towait.tv_sec - apres.tv_sec -1 >= TIMEOUTSEC && apres.tv_nsec - towait.tv_nsec > TIMEOUTUSEC*1000)
				{
					appel++;
					return false;
				}
			}
			else
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
	}
	// ODID special => deblocage
	else
	{
		flags |= GAOPSPE; //pour eviter que le mec qui a le numero appel + 1 emmete en meme temps que nous
		towait.tv_sec = 0;
		towait.tv_nsec = 50000; //50 microsecondes
		while (flags & GAOPSND)
			nanosleep(&towait, NULL); //qqn emmet => attente
		
	}

	// Emission et construction
	flags |= GAOPSND; //dire qu'il y a qqn qui emet
	octet buf[TAILLE_MAX_FRAME];

	int taille_trame = create_trame(buf, cmd, odid);
	
	int octets_envoyes = write(device, buf, taille_trame*sizeof(octet));
	//tcdrain(device); //attendre que c'est bien envoye
	
	#if DEBUG && !IAmNotOnThePandaBoard
		cout << "Nombre octets envoyés : " << octets_envoyes << endl;
	#endif

	//l'apres devient l'avant
	if (odid != ODIDSPECIAL)
	{
		if (++frames_envoyees >= NB_FRAMES_MAX)
			flags |= GAOPBLK; //on attend que la fonction distante se libere a nouveau pour reemettre
		
		appel++; //appel le suivant
	}
	else
		flags &= ~GAOPSPE; //fin de la requete de debloquage
	
	flags &= ~GAOPSND; //fin de l'emmission
	
	return (octets_envoyes == taille_trame);
}

bool PCGaop::receive(AssocPeriphOdid& tblassoc)
{
	Commande cmd;
	octet odid;
	octet buf[TAILLE_MAX_FRAME];
	int i, nb_donnees = 0;
	
	// On indique que l'on est prêt à recevoir si c'est débloqué
	if (flags & GAOPDBK)
	{
		send(cmd, ODIDSPECIAL);
		flags &= ~GAOPDBK;
		frames_recues = 0; //pret a recevoir
	} 

	do	
	{
		// Début d'une trame
		if (nb_donnees == 0)
		{
			// On boucle tant que l'on n'a pas un début de trame
			i = read(device, buf, 1*sizeof(octet));

			if (buf[0] != BEGIN_TRAME && i>= 0)
				continue;
		}
		// Récupération de l'entête
		else if (nb_donnees < INFOCPL_DEBUT)
		{ 
			i = read(device, buf+nb_donnees, (INFOCPL_DEBUT - nb_donnees)*(sizeof(octet)));
		}
		else //lit le reste = data + fin
			i = read(device, buf+nb_donnees, (buf[2]+INFOCPL_FIN)*(sizeof(octet)));
			
		nb_donnees += i;
		/*
			FIXME: Fail de lecture répétées
		*/
		struct timespec towait;
		towait.tv_sec = 0;
		towait.tv_nsec = 50000; //50 microsecondes
		nanosleep(&towait, NULL);

#ifdef DEBUG
				cout << "DEBUG PCGaop::send : Nombre de données lues via read : " << nb_donnees << endl;
		cout << "Derniere valeur de i : " << i << endl;
		if (nb_donnees >= 2 )
			cout << "Nombre de données attendues : " << (buf[2] + INFOCPL) <<endl;
#endif
		
		if (nb_donnees == 0 || i < 0 || nb_donnees > TAILLE_MAX_FRAME)
		{
#ifdef DEBUG
			cout << "DEBUG PCGaop::send : fail de read" << endl;
#endif
			return false;
		}
	} while (nb_donnees != (buf[2]+INFOCPL) && i >= 0);

	// On essaye de lire cette trame
	if ( read_trame(buf, cmd, odid) )
	{ 
		#if DEBUG
			cout << "DEBUG PCGaop::send : Succès de lecture de la trame" <<endl;
		#endif

		if (odid == ODIDSPECIAL)
		{
		#if DEBUG
			cout << "DEBUG PCGaop::send : ODID spécial" <<endl;
		#endif

			flags &= ~GAOPBLK;
			frames_envoyees = 0;
		} 
		else if (tblassoc.getByODID(odid) != NULL)
		{
			if (++frames_recues >= NB_FRAMES_MAX) flags |= GAOPDBK;
			tblassoc.getByODID(odid)->receive(cmd);
		}
		
		return true;
	}
	else
	{
		std::cerr << "Erreur de transmition ! " << std::endl;
		return false;
	}
}
