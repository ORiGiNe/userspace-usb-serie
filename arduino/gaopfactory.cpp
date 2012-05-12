#include "gaopfactory.h"
#include "ArduinoGaop.h"

AbstractGaop* GaopFactory::createGaop(const char *device)
{
	return new ArduinoGaop();
}
