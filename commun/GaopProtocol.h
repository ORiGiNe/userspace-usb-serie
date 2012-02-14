#ifndef GAOPPROTOCOL
#define GAOPPROTOCOL

/* Cet objet implemente le protocol GAOP pour la panda et pour les arduinios. 
 * Le code se veut commun aux slaves et aux devices. Un adaptateur de type 
 * sera eventuellement mis place cote de l'arduino/fpga. 
 */
/*  La trame est cree au fur est a mesure de l'envoie.
 *  Il y a une trame par commande.
 *  Exemple : envoye(a_quoi: moteur, arg1, arg2, ...)
 *  detail du protocol GAOP, GAOP is an origine protocol :
 *  Tous les champs seront codes sur 8bits, soit un octet.
 *  taille de la frame
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
 */
/* on a la bonne frame ssi l'octet 0 == DEBUT et le dernier octet lu == FIN */

#include "Peripherique.h" /*Gaop connait peripherique*/
#include "AbstractGaop.h" /*pour que peripherique puisse connaitre du GAOP*/

#define BUF_MAX 128 /* la pile de transfert usb de l'arduino ne peut contenir que 128 octets (cf doc arduino)*/

#ifndef IAmNotOnThePandaBoard
	#include <sys/stat.h>	/*open*/
	#include <fcntl.h>		/*open*/
	#include <sys/types.h>	/*kill*/
	#include <signal.h>		/*kill*/
	#include <unistd.h>		/*read, write, close, fork*/
	#include <termios.h>	/*tcgetattr, cfsetospeed, cfsetispeed, tcsetattr, tcdrain struct termios*/
	#include <time.h>		/*clock_gettime (implique -lrt), nanosleep*/ 
	#include <iostream>		/*cerr, cout, endl*/
	#include <cstring>		/*strerr*/
	#include <cerrno>		/*errno*/
#else
	#include <WProgram.h> //to have HIGH, LOW, digitalWrite, digitalRead, Serial.*, ...
#endif

/* Une commande est vu comme une chaine de carartere. */
/* Chaque peripheriques < devices > connait un gaop (passer via la fonction initialize) */
class Gaop : public AbstractGaop
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
		bool Receive(AssocPeriphOdid&);
		
		/* 
		 * initialise la connection et recupere  ce qui marche sur l'arduino. 
		 * p est un tableau contenant l'odid de tout les peripheriques 
		 */
		//void initialise(short int odidPeripheriques[] , int taille); //odidPeripheriques valeur = -1 <=> peripherique desactive
		void initialise(AssocPeriphOdid&);
	private:
		#ifndef IAmNotOnThePandaBoard
			int device; //file descriptor (open function)
			int pid_fils; //pour le fork de Receive();
		#endif
		/* Le drivers est thread safe. on met en place un systeme de priorite du
		 * du style premier arrive = premier servi. */
		octet prochain; //prochain numero disponible (% 256)
		octet appel; //candidat appele
		bool blocked; //bloquer les emissions lorsque c'est indisponible de l'autre cotgge
};

#endif /*GAOPPROTOCOL */
