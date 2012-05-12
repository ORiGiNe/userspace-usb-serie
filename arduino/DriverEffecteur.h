#ifndef DRIVEREFFECTEUR_H
#define DRIVEREFFECTEUR_H

#include <WProgram.h>

class DriverEffecteur
{
	public:
		virtual bool set(int val) = 0;
};

#endif /*DRIVEREFFECTEUR*/
