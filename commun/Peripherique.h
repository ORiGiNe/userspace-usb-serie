#ifndef PERIPHERIQUE_H
#define PERIPHERIQUE_H

#include "AbstractGaop.h"
#include "Commande.h"
//TODO:doxygen
/* Cette classe est une classe generique de peripherique tout peripherique
 * Le code de ce bidule sera different cote arduino et cote pc
 */

// Ne compile pas sans ces 3 lignes FIXME
#if IAmNotOnThePandaBoard
extern "C" void __cxa_pure_virtual();
#endif

class Peripherique
{
	public:
		Peripherique(octet odid); //constructeur par default temporaire -> URBI
		
		octet getOdid();
		
		bool test(); //test le fonctionnement. Renvoie faux si ne marche pas
		bool operation(); //fait l'operation distante
		
		/* N'ont pas besoin d'etre wrapper sous urbi */
		virtual void receive(Commande&) = 0; //peripherique recoit des donnees

		/**
			* @param g
			Gaop a associer au peripherique
		*/
		void associe(AbstractGaop* g); 
	protected:
		octet odid; //Numero unique identifiant le peripherique
		Commande cmd;
	private:
		AbstractGaop* g;

};

#endif /* OrIgInE_Device */

