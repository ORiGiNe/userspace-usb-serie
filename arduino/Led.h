#ifndef LED_H
#define LED_H

#include "DriverEffecteur.h"

class Led : public DriverEffecteur
{
	public:
		Led(int pin);
		bool set(int); //0 : eteint. 1 si allumer
	private:
		int pin;
};

#endif /*LEDOrIgInE*/
