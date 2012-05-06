#include "ClassServo.h"
#include "/usr/share/arduino/libraries/Servo/Servo.h"

EffecteurServo::EffecteurServo(int pin[], int nbr)
{
				for (int i = 0; i < nbr; i++)
				{
								this->pin[i] = pin[i];
				}

				this->pos = 0;
				this->nbr = nbr;
}

bool EffecteurServo::setup()
{
  for (int i = 0; i < this->nbr; i++)
	{
		this->myservo[i].attach(this->pin[i]); //attaches the servo on pin to the servo object (pin 14 = analog 0);
	}
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

            for (int i = 0; i < this->nbr; i++)
						{
						  myservo[i].write(this->pos);
						}
						
            delay(15);
          }
          
        return true;
}

