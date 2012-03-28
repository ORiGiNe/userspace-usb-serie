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
 *  taille de la frame					//8 bits
 *  odid du peripherique				//16 bits
 *  octet 1.1							//8 bits =>1.1 et 1.2 definissient un ushort
 *  octet 1.2							//8 bits
 *  octet 2.1							//8 bits
 *  octet 2.2							//8 bits
 *  ...
 *  checksum (ou checkxor plutot)		//8 bits
 */
/* on a la bonne frame ssi l'octet 0 == DEBUT et le dernier octet lu == FIN */

#include "Peripherique.h" /*Gaop connait peripherique*/
#include "AbstractGaop.h" /*pour que peripherique puisse connaitre du GAOP*/

#if !IAmNotOnThePandaBoard
	#include <sys/stat.h>	/*open*/
	#include <fcntl.h>		/*open*/
//	#include <sys/types.h>	/*kill*/
	#include <pthread.h>	/*pthread_* */
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

/* Definition des flags pour la gestions des threads "mutex" */
#define GAOPBLK 0b00000001 /*0x01 emissions bloquees*/
#define GAOPDBK 0b00000010 /*0x02 emissions de l'autre machine (distante bloquees) */
#define GAOPSND 0b00000100 /*0x04 emission en cours */
#define GAOPSPE 0b00001000 /*0x08 trame speciale en cours pour debloquer (odid == 0xFF) */

/* Une commande est vu comme une chaine de carartere. */
/* Chaque peripheriques < devices > connait un gaop (passer via la fonction initialise) */
class Gaop : public AbstractGaop
{
	public:
		#if !IAmNotOnThePandaBoard
                Gaop(const char *device); //"/dev/ttyACM0"
		#else
                Gaop();
        #endif
        ~Gaop();
		
		/* 
		 * initialise la connection et recupere  ce qui marche sur l'arduino. 
		 * p est un tableau contenant l'odid de tout les peripheriques
		 * appele apres la declaration des peripheriques.
		 */
		//void initialise(short int odidPeripheriques[] , int taille); //odidPeripheriques valeur = -1 <=> peripherique desactive
		void initialise(AssocPeriphOdid*);

	/* n'on pas besoin d'etre wrapper sous urbi. */

		/*
		 * envoie les commandes contenues dans c. odid et l'identifiant
		 * de l'emetteur, qui permet de connaitre aussi le destinataire 
		 * renvoie false si erreur d'envoie
		 */
		bool Send(Commande &c, unsigned short int odid); 		
		
		/*
		 * reception des commandes dans l'objet c
		 * odid : recoit l'odid du peripherique auquel les donnees sont destinee
		 * renvoie false si erreur de reception
		 */
		bool Receive(AssocPeriphOdid&);
		
	private:
		#if !IAmNotOnThePandaBoard
			int device; //file descriptor (open function)
			pthread_t fils; //pour le fork de Receive();
			void **pthreadarg;
		#endif
		/* Le drivers est thread safe. on met en place un systeme de priorite du
		 * du style premier arrive = premier servi. */
		octet prochain; //prochain numero disponible (% 256)
		octet appel; //candidat appele
		octet flags; //GAOPSPE, GAOPBLK, GAOPDBK, GAOPSND
		octet frames_envoyees;
		octet frames_recues;

};

#endif /*GAOPPROTOCOL */
