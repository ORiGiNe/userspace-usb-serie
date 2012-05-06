#ifndef ServoOrIgInE
#define ServoOrIgInE
#include "DriverEffecteur.h" 
#include <WProgram.h>
#include "/usr/share/arduino/libraries/Servo/Servo.h"

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

#endif /*ServoOrIgInE*/
