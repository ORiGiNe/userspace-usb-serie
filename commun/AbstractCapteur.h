#ifndef ABSTRACTCAPTEUR
#define ABSTRACTCAPTEUR

#include "DriverCapteur.h"

class AbstractCapteur
{
	public:
		virtual int get() = 0;
	protected:
		DriverCapteur* driver;
};

#endif /*ABSTRACTCAPTEUR*/

