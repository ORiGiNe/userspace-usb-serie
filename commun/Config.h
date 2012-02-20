#ifndef FICHIERDECONFIGURATION
#define FICHIERDECONFIGURATION

/* Indique si le code tourne sur arduino ou sur pc : include differents. 
 * Decommenter la ligne si le programme tourne sur arduino */
//#define IAmNotOnThePandaBoard


/* delai de timeout */
//  secondes
#define TIMEOUTSEC 0
// microseconde
#define TIMEOUTUSEC 10000

/* taille maximal de la frame (<= a 128, car apres buffer arduino plein)*/
#define TAILLE_MAX_FRAME 128


/* odid special utilise pour que le gaop du pc et le gaop de l'arduino communique entre eux
 * (frame d'authorisation d'emmission). L'odid est code sur 16 bit */
#define ODIDSPECIAL 0xFFFF

/* taille des infromations complemetaire : 
 * taille	1
 * odid		2
 * checksum	1 */
#define INFOCPL 4

#endif /*FICHIERDECONFIGURATION*/
