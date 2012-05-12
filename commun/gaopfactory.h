#ifndef GAOPFACTORY_H
#define GAOPFACTORY_H

#include "AbstractGaop.h"
#include <stdlib.h>

class GaopFactory
{
	public:
		static AbstractGaop* createGaop(const char *device = NULL);
};

#endif // GAOPFACTORY_H
