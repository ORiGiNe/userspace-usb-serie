#ifndef GAOPPROTOCOL_H
#define GAOPPROTOCOL_H

#include "AbstractGaop.h"
#include "AssocPeriphOdid.h"

/*!
 *	\class Gaop
 *	\brief GAOP is An ORiGiNe protocole
 *	GAOP coté arduino
 */
class ArduinoGaop : public AbstractGaop
{
	public:
		ArduinoGaop();
		~ArduinoGaop();

		void initialise(AssocPeriphOdid*);
		bool send(Commande &c, octet odid);
		bool receive(AssocPeriphOdid&);
};

#endif /*GAOPPROTOCOL */
