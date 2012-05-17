#ifndef ASSERV_H
#define ASSERV_H

#include "Peripherique.h"

class Asserv : public Peripherique
{
	public:
		Asserv(octet odid);
		/*!
		 * Valeur à laquelle on veut avancer. Non-bloquante
		 */
		bool avancer(int distance, int vitesse, int acceleration);
		/*!
		 * Valeur à laquelle on veut tourner. Non-bloquante
		 */
		bool tourner(int angle, int vitesse, int acceleration);
		/*!
		 * Commande pour s'arreter net (comme les musettes). Non-bloquante
		 */
		bool arreter();
		/*!
		 * Commande pour récupérer la dernière distance (ie dernier appel de distance ou arret)
		 */
		int getLastDistance();
		/*!
		 * Commande pour récupérer la dernière rotation (ie dernier appel de rotation ou arret)
		 */
		int getLastRotation();

		/*!
		 * Récupération de la commande brute
		 */
		void receive(Commande&);
	private:
		/*!
		 * Fonction permettant de gérer un timeout pour tous les getters
		 */
		Commande get();

		bool reponse;
};

#endif
