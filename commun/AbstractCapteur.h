#ifndef ABSTRACTCAPTEUR
#define ABSTRACTCAPTEUR

#include "Capteur.h"

class AbstractCapteur
{
	public:
		virtual int get() = 0;
	protected:
		Capteur* driver;
};

#endif /*ABSTRACTCAPTEUR*/

