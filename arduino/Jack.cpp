#include "Jack.h"
#include <WProgram.h>

// Macro-fonctions pour renvoyer les valeurs dans l'unité voulue
#define timeToCm(time) ((time) / 29 / 2)
#define timeToMm(time) (((time)*10) / 29 / 2)

Jack::Jack(int _pin)
{
	pin = _pin;

	// Define in lib arduino
	pinMode(pin, INPUT);
}

int Jack::get()
{
	return digitalRead(pin) == HIGH;
}
