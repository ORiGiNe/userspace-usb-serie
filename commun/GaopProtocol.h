#ifndef GAOPPROTOCOL
#define GAOPPROTOCOL
//#define IAmNotOnThePandaBoard

/* Cet objet implemente le protocol GAOP pour la panda et pour les arduinios. 
 * Le code se veut commun aux slaves et aux devices. Un adaptateur de type 
 * sera eventuellement mis place cote de l'arduino/fpga. 
 */
/*  La trame est cree au fur est a mesure de l'envoie.
 *  Il y a une trame par commande.
 *  Exemple : envoye(a_quoi: moteur, arg1, arg2, ...)
 *  detail du protocol GAOP, GAOP is an origine protocol :
 *  Tout les champs seront codee sur 8bits, soit un octet.
 *  //octet de debut
 *  nombre de donnees = n
 *  odid du peripherique
 *  taille de la première donnee = t1
 *  octet 1
 *  ...
 *  octet t1
 *  taille de la deuxieme donnee = t2
 *  ...
 *  ...
 *  taille de la n-ieme donnee = tn
 *  octet 1
 *  ...
 *  octet tn
 *  checksum (ou checkxor plutot)
 *  //octet de fin
 */

//#define DEBUT 0x00
//#define FIN 0xFF

#include "Commande.h" /* type octet et objet Commande */

#define BUF_MAX 128 /* la pile de transfert usb de l'arduino ne peut contenir que 128 octets (cf doc arduino)*/

#ifndef IAmNotOnThePandaBoard
	#include <sys/stat.h>	/*open*/
	#include <fcntl.h>		/*open*/
	#include <unistd.h>		/*read, write, close, sleep, usleep*/
	#include <termios.h>	/*tcgetattr, cfsetospeed, cfsetispeed, tcsetattr, struct termios*/
	#include <iostream>		/*cerr, cout, endl*/
	#include <cstring>		/*strerr*/
	#include <cerrno>		/*errno*/
#endif

/* Une commande est vu comme une chaine de carartere. */
/* Chaque peripheriques < devices > connait un drivers (passer en constructeur) */
/* L'odid 0 correspond a cet objet (GOAP) */
class Gaop
{
	public:
		#ifndef IAmNotOnThePandaBoard
                Gaop(const char *device); //"/dev/ttyACM0"
		#else
                Gaop();
        #endif
        ~Gaop();
		
		/*
		 * envoie les commandes contenues dans c. odid et l'identifiant
		 * de l'emetteur, qui permet de connaitre aussi le destinataire 
		 * renvoie false si erreur d'envoie
		 */
		bool Send(Commande &c, octet odid); 		
		
		/*
		 * reception des commandes dans l'objet c
		 * odid : recoit l'odid du peripherique auquel les donnees sont destinee
		 * renvoie false si erreur de reception
		 */
		bool Receive(Commande &c, octet &odid);
		
		/* 
		 * initialise la connection et recupere  ce qui marche sur l'arduino. 
		 * p est un tableau contenant l'odid de tout les peripheriques 
		 */
		void initialise(short int odidPeripheriques[] , int taille); //odidPeripheriques valeur = -1 <=> peripherique desactive
	private:
		#ifndef IAmNotOnThePandaBoard
			int device; //file descriptor
		#endif
};

#endif /*GAOPPROTOCOL */