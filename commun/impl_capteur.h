#ifndef CAPTEURS
#define CAPTEURS

#include "Peripherique.h"
#include "abstract_capteurs.h"
#include "DriverCapteur.h"

/* Cet objet est un capteur generique
 * Il contient la fonction get
 * Commportement de cette fonction : 
 *	PC:
 *		get: prend la valeur du capteur. 
 *	Arduino:
 *		la fonction devra etre implementee dans l'arduino pour querir les
 *		bonnes donnees. Elle sera appelee par la fonction Receive
 *		get : renvoie la valeur du capteur 
 *		=> { cmd[0] = <valeur>; }
 *	cmd[0] : utiliser pour passer la valeur
 */

//pour que l'on puisse appele le capteur
//les differents capteur (ultrasons, ...) herite de ca.
class Capteur : public AbstractCapteur
{
	public:
		Capteur(Drivers Capteur*, AssocPeriphOdid*);
		~Capteur();
		int get();
};

#endif /*CAPTEURS*/

