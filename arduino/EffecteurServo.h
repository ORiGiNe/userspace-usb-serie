#ifndef EFFECTEURSERVO_H
#define EFFECTEURSERVO_H

#include "DriverEffecteur.h"
#include <Servo.h>

class EffecteurServo : public DriverEffecteur
{
	public:
		EffecteurServo(int pin[], int nbr);
		bool set(int); //0 : replier. 1 deployer (= 90°)
		bool setup();
	private:
		int pin[5];
		Servo myservo[5];
		int pos; //to store the servo position
		int nbr;
};

#endif /*EFFECTEURSERVO_H*/
