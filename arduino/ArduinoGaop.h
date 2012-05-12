#ifndef GAOPPROTOCOL
#define GAOPPROTOCOL

#include "Peripherique.h" 
#include "AbstractGaop.h"
#include <WProgram.h> 

/*!
 *	\class Gaop
 *	\brief GAOP is An ORiGiNe protocole
 *	GAOP coté arduino
 */
class Gaop : public AbstractGaop
{
	public:
		Gaop();
    
		~Gaop();
		
		void initialise(AssocPeriphOdid*);

		bool Send(Commande &c, unsigned short int odid); 		
		
		bool Receive(AssocPeriphOdid&);
};

#endif /*GAOPPROTOCOL */
