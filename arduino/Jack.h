#ifndef JACK_H
#define JACK_H

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
