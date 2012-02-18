#ifndef LEDOrIgInE
#define LEDOrIgInE
#include "Peripherique.h"

class Led : public Peripherique
{
	public:
		Led(int odid, int pin);
        bool test();
        void Receive(Commande&);
		void allume();
		void eteint();
	private:
		int pin;
};

#endif /*LEDOrIgInE*/
