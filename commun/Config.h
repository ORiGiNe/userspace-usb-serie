#ifndef FICHIERDECONFIGURATION
#define FICHIERDECONFIGURATION

/* Indique si le code tourne sur arduino ou sur pc : include differents. 
 * Decommenter la ligne si le programme tourne sur arduino */
//#define IAmNotOnThePandaBoard


/* delai de timeout */
//  secondes
#define TIMEOUTSEC 1
// microseconde
#define TIMEOUTUSEC 10000

/* taille maximal de la frame (<= a 128, car apres buffer arduino plein)*/
#define TAILLE_MAX_FRAME 128

#endif /*FICHIERDECONFIGURATION*/
