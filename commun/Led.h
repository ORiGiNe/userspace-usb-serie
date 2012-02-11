#ifndef LEDOrIgInE
#define LEDOrIgInE
#include "Peripherique.h"

class Led : public Peripherique
{
	public:
		Led(octet odid, octet pin);
		void allume();
		void eteint();
        bool test();
        void Receive(Commande&);
	private:
		octet pin;
};

#endif /*LEDOrIgInE*/
