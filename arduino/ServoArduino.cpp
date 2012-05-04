#include "ClassServo.h"
#include "/usr/share/arduino/libraries/Servo/Servo.h"

EffecteurServo::EffecteurServo(int pin)
{
        this->pin = pin;
	this->pos = 0; 
}

bool EffecteurServo::setup()
{
	this->myservo.attach(pin); //attaches the servo on pin to the servo object (pin 14 = analog 0);
}


bool EffecteurServo::set(int val)
{
        if ( val < 0 || val > 180 )
          return false;

          while (this->pos != val)	//sweep the position
          {
            if (this->pos < val)
              this->pos++; //step : 1 degree              
            else
              this->pos--;
            myservo.write(this->pos);
            delay(15);
          }
          
        return true;
}

