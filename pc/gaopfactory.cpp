#include "gaopfactory.h"
#include "PCGaop.h"

AbstractGaop* GaopFactory::CreateGaop(const char *device)
{
	return new PCGaop(device);
}

