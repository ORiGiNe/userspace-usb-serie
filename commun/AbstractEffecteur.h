#ifndef ABSTRACTEFFECTEUR
#define ABSTRACTEFFECTEUR

#include "Effecteur.h"

class AbstractEffecteur
{
	public:
		virtual bool set(int valuetoset) = 0;
		virtual bool test() = 0;
	protected:
		Effecteur* driver;
};

#endif
