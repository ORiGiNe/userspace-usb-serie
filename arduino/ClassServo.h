#ifndef ServoOrIgInE
#define ServoOrIgInE
#include "DriverEffecteur.h" 
#include <WProgram.h>
#include "/usr/share/arduino/libraries/Servo/Servo.h"

class EffecteurServo : public DriverEffecteur
{
	public:
		EffecteurServo(int pin);
		bool set(int); //0 : replier. 1 deployer (= 90°)
		bool setup();
	private:
		int pin;
		Servo myservo;
		int pos; //to store the servo position
};

#endif /*ServoOrIgInE*/
