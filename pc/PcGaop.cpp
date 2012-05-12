#include "PCGaop.h"

#include <sys/stat.h>	/*open*/
#include <fcntl.h>		/*open*/
#include <signal.h>		/*kill*/
#include <unistd.h>		/*read, write, close, fork*/
#include <termios.h>	/*tcgetattr, cfsetospeed, cfsetispeed, tcsetattr, tcdrain struct termios*/
#include <time.h>		/*clock_gettime (implique -lrt), nanosleep*/
#include <iostream>		/*cerr, cout, endl*/
#include <cstring>		/*strerr*/
#include <cerrno>		/*errno*/
#include <WProgram.h> //to have HIGH, LOW, digitalWrite, digitalRead, Serial.*, ...

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
 * Thread qui appellera en boucle la fonction Receive du GAOP
 * avec son tableau d'ODID.
 * Ce thread est tué par le destructeur du GAOP
 */
void* run_gaop(void* arg)
{
	struct timespec t = {0, 10000}; //10 microsecondes
	void **argv = (void**)arg;
	
	while (true)
	{
		((Gaop*)(argv[0]))->
				Receive( *((AssocPeriphOdid*)(argv[1])) );
		nanosleep(&t, NULL);
	}
	
	return NULL;
}

void PCGaop::initialise(AssocPeriphOdid *tblassoc)
{
	/* DEBUG: afficher les devices enregistrées et les trames attendues IMPOSSIBRU */
	/*#ifdef DEBUG
		for (int i = 0 ; i < tblassoc->taille ; i++)
		{
	*/
	/* demade au slave (aduino) des informations sur ces devices (capteurs / effecteurs) */
	octet r[1] =  {0}; // FIXME Remplacer par un pointeur vers un octet, au moins, on sait ce qu'on fait comme ça
	int x = 0; // x correspond au nombre de read sans rien à la base, mais plus à rien maintenant
	octet odid;

	//attente d'une connexion
	while (r[0] == 0 && x < 100)
	{
		write(device, ">", 1);	//Je suis demare. Tu es pret ?
		while(read(device, r, 1) == 0);	//Oui
		x++;
	} // FIXME x ne peut pas etre superieur à 1, à cause du while | Skelz : agreed, on jarte
	//	if (x > 100) throw "connection timed out";
	write(device, "?", 1);	//Tu as combien de peripheriques ? // FIXME, voir wiki pour voir si c'est le vrai pb
	while (read(device, r, 1) == 0);		//J'en ai x
	x = r[0];
	for (int i = 0; i < x; i++)
	{
		//Donne moi l'ODID du device i si il marche, 0 sinon
		write(device, "i", 1);
		//tiens, voila
		while( read(device, r, 1) == 0);
		odid = r[0]; // FIXME l'identifiant est codé sur 1 octet et non 2 (pour les specs, c'est 2 à ce qu'on m'a dit)

		if (tblassoc->getbyodid(odid) != NULL) //je regarde si je le connais
		{
			write(device, "t", 1); // test son fonctionnement (et desactive le si il ne marche pas) XXX:magie du saint-esprit ?
			while( read(device, r, 1) == 0);
			if (r[0] != 'y') //si ca marche pas
				tblassoc->rm(odid); //suppression par odid
			else
				//ca marche bien. je dis au peripherique qu'il peut m'utiliser
				tblassoc->getbyodid(odid)->associe(this);
		}
		else
			write(device, "x", 1); //je ne le connais pas. desactive le
	}

	//on thread la fonction receive
	pthreadarg[0] = this;
	pthreadarg[1] = tblassoc;
	pthread_create(&fils, NULL, run_gaop, pthreadarg);
}

bool PCGaop::Send(Commande &cmd, octet odid)
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
	octet buf[TAILLE_MAX_FRAME]; //on a besoin de qqch de rapide, pas de qqch d'elegant -> pas d'allocation dynamique.

	int taille_trame = create_trame(buf, cmd, odid);
	
	int octets_envoyes = write(device, buf, taille_trame*sizeof(octet));
	//tcdrain(device); //attendre que c'est bien envoye
	
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
	
	return (octets_envoye == taille_trame);
}

bool PCGaop::Receive(AssocPeriphOdid& tblassoc)
{
	Commande cmd;
	octet odid;
	//int ind_buf = 0;
	octet buf[TAILLE_MAX_FRAME];
	int i, nb_donnees;
	
	if (flags & GAOPDBK)
	{
		Send(cmd, ODIDSPECIAL);
		flags &= ~GAOPDBK;
		frames_recues = 0; //pret a recevoir
	} 
	
	do	//attendre que les octets arrivent
	{
		if (nb_donnees == 0)
		{
			i = read(device, buf, 1);
			if (buf[0] != BEGIN_FRAME && i>= 0) continue; //syncronisation sur une debut
		}
		else if (nb_donnees < 4)
		{ //recuperation de l'entete
			i = read(device, buf+1, 4 - nb_donnees)
		}
		else //lit le reste = data + fin
			i = read(device, buf+nb_donnees, (buf[2]+2)*(sizeof(octet)));
			
		nb_donnees += i;
		
		if (nb_donnees == 0 || i < 0 || nb_donnees > TAILLE_MAX_FRAME)
		{
			return false;
		}
	} while (nb_donnees != buf[0] && i >= 0);


	if (read_trame(trame, nb_donnees, cmd, odid))
	{ //trame OK
		if (odid == ODIDSPECIAL)
		{
			flags &= ~GAOPBLK;
			frames_envoyees = 0;
		} //frame pour dire que l'on peut envoye
		else if (tblassoc.getbyodid(odid) != NULL)
		{
			if (++frames_recues >= NB_FRAMES_MAX) flags |= GAOPDBK;
			tblassoc.getbyodid(odid)->Receive(cmd);
		}
		return true;
	}
	else
	{
		std::cerr << "Erreur de transmition ! " << std::endl;
		return false;
	}
}
