#ifndef DRIVERCAPTEUR_H
#define DRIVERCAPTEUR_H

#include <Arduino.h>

class DriverCapteur
{
	public:
		virtual int get()=0;
};

#endif /*DRIVERCAPTEUR*/
