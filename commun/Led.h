#ifndef LEDOrIgInE
#define LEDOrIgInE
#include "DriverEffecteur.h" 

class Led : public DriverEffecteur
{
	public:
		Led(int odid, int pin);
        bool test();
		bool set(int); //0 : eteint. 1 si allumer
	private:
		int pin;
};

#endif /*LEDOrIgInE*/
