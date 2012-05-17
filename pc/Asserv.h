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
		void avancer(int distance);
		/*!
		 * Valeur à laquelle on veut tourner. Non-bloquante
		 */
		void tourner(int angle);
		/*!
		 * Commande pour s'arreter net (comme les musettes). Non-bloquante
		 */
		void arret();
		/*!
		 * Commande pour récupérer la dernière distance (ie dernier appel de distance ou arret)
		 */
		int getLastDistance();
		/*!
		 * Commande pour récupérer la dernière rotation (ie dernier appel de rotation ou arret)
		 */
		int getLastRotation();

		// Fonction propre au framework
		void receive(Commande&);
	private:

}
