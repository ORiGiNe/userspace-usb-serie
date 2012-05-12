#ifndef GAOPFACTORY_H
#define GAOPFACTORY_H

#include "AbstractGaop.h"

class GaopFactory
{
	public:
		static AbstractGaop* CreateGaop(const char *device = null);
};

#endif // GAOPFACTORY_H
