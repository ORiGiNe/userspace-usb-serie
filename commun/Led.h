#ifndef LEDOrIgInE
#define LEDOrIgInE
#include "Peripherique.h"

class Led : public Peripherique
{
	public:
		Led(octet odid, Gaop &g);
		void allume();
		void eteint();
                bool test();
                void Receive(Commande&);
};

#endif /*LEDOrIgInE*/
