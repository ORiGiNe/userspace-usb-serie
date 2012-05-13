#ifndef DRIVERCAPTEUR_H
#define DRIVERCAPTEUR_H

#include <WProgram.h>

class DriverCapteur
{
	public:
		virtual int get()=0;
};

#endif /*DRIVERCAPTEUR*/
