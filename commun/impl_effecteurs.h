#ifndef GAOPCAPTEURSEFFECTEURS
#define GAOPCAPTEURSEFFECTEURS

#include "Peripherique.h"
#include "abstract_effecteurs.h"
#include "DriverEffecteur.h"

/* Cet objet est un capteur effecteur generique
 * Il contient la fonctions get set
 * Commportement de cette fonction : 
 *	PC:
 *		set:
 *			defini la valeur de l'effecteur.
 *			set envoie selement si l'argument est dans les limites 
 *	Arduino:
 *		la fonction devra etre implementee dans l'arduino pour querir les
 *		bonnes donnees. Elle sera appelee par la fonction receive
 *		set:
 *			definit la valeur du capteur
 *	cmd[0] : utiliser pour passer la valeur
 */

class AbstractEffecteur : public Peripherique
{
	public:
		Effecteur(DriverEffecteur*, AssocPeriphOdid*);
		void set(int);

};

#endif

