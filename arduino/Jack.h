#ifndef JACK
#define JACK

#include "DriverCapteur.h"

class Jack : public DriverCapteur
{
	public:
		Jack(int pin);
		int get();
	private:
		int pin;
};
#endif /*JACK*/
