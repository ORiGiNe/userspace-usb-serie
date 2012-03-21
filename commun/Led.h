#ifndef LEDOrIgInE
#define LEDOrIgInE
#include "Effecteur.h" 

class Led : public Effecteur
{
	public:
		Led(int odid, int pin);
        bool test();
		bool set(int); //0 : eteint. 1 si allumer
	private:
		int pin;
};

#endif /*LEDOrIgInE*/
