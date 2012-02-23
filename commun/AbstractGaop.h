#ifndef ABSTRACTGAOPPROTOCOL
#define ABSTRACTGAOPPROTOCOL

#include "Config.h"
#include "Commande.h" /* type octet et objet Commande */


/* l'utilite de ceci ce justifie par le fait que Gaop a besoin de connaitre peripherique, 
 * mais Peripherique a aussi besoin de connaitre Gaop. Cela evite les inclusions mutuelle */
class AbstractGaop
{
	public:
		
		/*
		 * envoie les commandes contenues dans c. odid et l'identifiant
		 * de l'emetteur, qui permet de connaitre aussi le destinataire 
		 * renvoie false si erreur d'envoie
		 */
		virtual bool Send(Commande &c, unsigned short int odid) = 0; 		
		
};

#endif /*ABSTRACTGAOPPROTOCOL*/
