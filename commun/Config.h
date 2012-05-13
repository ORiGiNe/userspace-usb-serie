#ifndef FICHIERDECONFIGURATION
#define FICHIERDECONFIGURATION

/*!
 *	\file Config.h
 *	\brief Header de configuration global (pc et arduino)
 *	\author L. Delmaire & R. Malmartel
 */

/*!
 *	\def	ARDUINO
 *	\brief	Indique si le code tourne sur arduino ou sur pc : include differents
 * 	et hacks trickys pour faire tourner du cpp sous l'arduino.
 *	A la compilation sur le pc, on passe la valeur de cette macro à 0 (-D inside).
 */
#ifndef ARDUINO
#define ARDUINO 1
#endif /* ARDUINO */

/*! 
 *	\brief Définition des structures de bases utilisées dans le GAOP
 */

/**	 
 *	\struct octet
 *	Définition d'un octet
 */
typedef unsigned char octet;
/*!
 *	\def BEGIN_TRAME
 *	\brief Octet de début de trame
*/
#define BEGIN_TRAME 0xFF

/*!
 *	\def END_TRAME
 *	\brief Octet de début de trame
*/
#define END_TRAME 0xFF

/*!
 *	\def INFOCPL_DEBUT
 *	\brief Entete d'une trame
 * Il s'agit de
 * debut 1o
 * taille	1o
 * odid		1o
 * séquence 1o
*/
#define INFOCPL_DEBUT 4 

/*!
 *	\def INFOCPL_DEBUT
 *	\brief Entete d'une trame
 * Il s'agit de
 * checksum 1o
 * debut 1o
*/
#define INFOCPL_FIN 2 

/*!
 *	\def INFOCPL
 *	\brief Taille en octet des informations complémentaires
 *	Il s'agit de :
 *  * debut et fin 2o
 */
#define INFOCPL INFOCPL_DEBUT + INFOCPL_FIN

/*!
 *	\def COMMAND_SIZE
 *	\brief Taille d'une commande
 *	Défini la taille d'une commande en octet 
*/
#define COMMAND_SIZE 2

/*!
 *	\def NBR_COMMAND_FRAME
 *	\brief Nombre de commandes par frame
*/
#define NBR_COMMAND_FRAME_MAX 6

/*!
	* \def TAILLE_MAX_FRAME
	*	\brief Taille maximum autorisée pour une frame
	* Cette valeur dépend de la constante INFOCPL, et du nombre de commandes
	*	par frames.
*/
#define TAILLE_MAX_FRAME (COMMAND_SIZE*NBR_COMMAND_FRAME_MAX)+INFOCPL

/*!
*	\def NB_FRAMES_MAX
*	\brief Nombre de frames maximum
*	Défini le nombre de frames envoyées avant un blocage des émissions, évitant ainsi une surcharge de buffer.
* A titre d'exemplen l'arduino UNO possède un buffer de taille 128 octets
*/
#define NB_FRAMES_MAX 10

/*!
 *	\struct trame
 * 	\brief Format de trame
 *	Une trame se compose de la manière suivante :
 * D = 1o : constante de debut
 * seq = 1o : n° sequence => relation d'ordre
 * t = 1o : taille data
 * CS = 1o : checksum == checkxor
 * odid = 1o : odid du periph
 * data = t*1o
 * F = 1o : constante de fin
 **/
typedef struct trame Trame;
struct trame {
		octet cmd[TAILLE_MAX_FRAME];
		octet odid;
		octet size;
		octet seq;
};

/*!
 *	\struct listedLine
 *	\brief Définition d'une liste chainée pour manipulation aisée des trames
 */
/*
typedef struct listedLine ListedLine;
struct listedLine {
		Trame trame;
		ListedLine* next;
		octet number;
};
*/

//TODO:MOVE ME ON Gaop.h
/*!
 *	\brief Definition des flags pour la gestions des threads "mutex"
 */
/*!
 *	\def GAOPBLK
 *	\brief Émission bloquées
 */
#define GAOPBLK 0b00000001
/*!
 *	\def GAOPDBK
 *	\brief Émission de l'autre machine (distante bloquée)
 */
#define GAOPDBK 0b00000010
/*!
 *	\def GAOPSND
 *	\brief Émission en cours
 */
#define GAOPSND 0b00000100 
/*!
 *	\def GAOPSPE
 *	\brief Trame spéciale de déblocage
 *	On envoi une trame ayant l'ODID 0xFF pour débloquer une potentielle erreur
 */
#define GAOPSPE 0b00001000
/*! 
 * \def ODIDSPECIAL
 * ODID spécial utilisé pour que le GAOP du pc et le GAOP de l'arduino communiquent entre eux
 * (frame d'authorisation d'émission).
 * L'odid est code sur 8 bits.
 */
#define ODIDSPECIAL 0xFF

/*!
 *	\def TIMEOUTSEC
 *	\brief Delai de timeout en seconde
 *	Ce délai doit etre suffisament long pour que les capteurs ai le temps de répondre
 */
#define TIMEOUTSEC 1
/**
 *	\def TIMEOUTUSEC
 *	\brief Delai de timeout en µ-seconde
 *	Ce délai doit etre suffisament long pour que les capteurs ai le temps de répondre
 */
//FIXME:wtf != ?
#define TIMEOUTUSEC 25000

#endif /*FICHIERDECONFIGURATION*/
