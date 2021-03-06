#include "TriggerEcho.h"
#include <Arduino.h>

// Macro-fonctions pour renvoyer les valeurs dans l'unité voulue
#define timeToCm(time) ((time) / 29 / 2)
#define timeToMm(time) (((time)*10) / 29 / 2)

TriggerEcho::TriggerEcho(int _trigger, int _echo)
{
	trigger = _trigger;
	echo = _echo;

	// Define in lib arduino
	pinMode(echo, INPUT);
	pinMode(trigger, OUTPUT);
}

int TriggerEcho::get()
{
	digitalWrite(this->trigger,LOW);
	delayMicroseconds(2);
	digitalWrite(this->trigger,HIGH);
	delayMicroseconds(10);
	digitalWrite(this->trigger,LOW);

	// Récupération de la longueur + insertion dans le tab
	return timeToCm(pulseIn(this->echo, HIGH));
}
