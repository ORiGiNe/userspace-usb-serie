#ifndef FICHIERDECONFIGURATION
#define FICHIERDECONFIGURATION

/* Indique si le code tourne sur arduino ou sur pc : include differents. 
 * Decommenter la ligne si le programme tourne sur arduino */
#ifndef IAmNotOnThePandaBoard
#define IAmNotOnThePandaBoard 1
#endif /* IAmNotOnThePandaBoard */


/* delai de timeout. doit etre suffisament long pour que les capteur est le temps de répondre*/
//  secondes
#define TIMEOUTSEC 1
// microseconde
#define TIMEOUTUSEC 25000

/* taille maximal de la frame  et nombre de frame maximum avant l'arret des emisions
 * TAILLE_MAX_FRAME defini le nombre d'octets maximum de la frame. (attention,
 * l'objet commande se defini une donnee sur 2 octets (short int)
 * NB_FRAMES_MAX defini le nombre de frame envoye avant un bloquage des
 * emissions evitant ainsi une surcharge du buffer. Attention, le buffer de
 * l'arduino est de 128 octets. donc, TAILLE_MAX_FRAME * NB_FRAMES_MAX < 128
 * plus NB_FRAMES_MAX et grand, plus le programme est rapide, mais mon la
 * longeur des donnees pouvant etre envoyee est importante*/
#define TAILLE_MAX_FRAME 14 /* 14 => 8 octets (14-INFOCPL) => 4 donnees (short int) */
#define NB_FRAMES_MAX 10

/* odid special utilise pour que le gaop du pc et le gaop de l'arduino communique entre eux
 * (frame d'authorisation d'emmission). L'odid est code sur 16 bit */
#define ODIDSPECIAL 0xFF

/* taille des infromations complemetaires : 
 * debut	1
 * taille	1
 * sequence 1
 * checksum	1 
 * odid		1
 * fin		1
 */
#define INFOCPL 6

#endif /*FICHIERDECONFIGURATION*/
