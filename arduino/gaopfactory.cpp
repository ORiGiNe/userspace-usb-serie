#include "gaopfactory.h"
#include "ArduinoGaop.h"

AbstractGaop* GaopFactory::CreateGaop(const char *device)
{
	return new ArduinoGaop();
}
