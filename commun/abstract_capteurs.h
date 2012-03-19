#ifndef ABSTRACTCAPTEURS
#define ABSTRACTCAPTEURS

/* Cet objet est un capteur virtuel generique
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
class AbstractCapteur
{
	public:
		//fonction a definir
		virtual int get()=0; //prendre sa valeur
};

#endif 

