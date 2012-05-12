#include "gaopfactory.h"
#include "PCGaop.h"

AbstractGaop* GaopFactory::createGaop(const char *device)
{
	return new PCGaop(device);
}

