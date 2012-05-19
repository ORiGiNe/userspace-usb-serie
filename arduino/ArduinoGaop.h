#ifndef GAOPPROTOCOL_H
#define GAOPPROTOCOL_H

#include "AbstractGaop.h"
#include "AssocPeriphOdid.h"

/*!
 *	\class Gaop
 *	\brief GAOP is An ORiGiNe protocole
 *	GAOP coté arduino.
 *	Il n'y a pas de thread (impossibru), ainsi le code est linéaire.
 */
class ArduinoGaop : public AbstractGaop
{
	public:
		ArduinoGaop();
		~ArduinoGaop();

		void initialise(AssocPeriphOdid*);
		bool send(Commande &cmd, octet odid);
		bool receive(AssocPeriphOdid&);

		/*!
		 * Lit une trame directement depuis l'instance Serial
		 * Cette méthode utilise AbstractGaop::read_trame et envoie un ack
		 * \param trame La trame à remplir
		 * \return Nombre de bytes lus
		 */
		int read_trame_from_serial(octet* trame);

		/*!
		 * Envoie une trame d'acknoledge
		 * \param ok : acknoledge ok si true, nook si false
		 * \param seq : sequence number
		 * \return : nb d'octets envoyees
		 */
		int send_ack(bool ok, int seq); 
};

#endif /* GAOPPROTOCOL */
