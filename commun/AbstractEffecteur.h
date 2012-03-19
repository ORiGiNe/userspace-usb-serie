#ifndef ABSTRACTEFFECTEUR
#define ABSTRACTEFFECTEUR

#include "DriverEffecteur.h"

class AbstractEffecteur
{
	public:
		virtual bool set(int valuetoset) = 0;
	protected:
		DriverEffecteur* driver;
};

#endif
