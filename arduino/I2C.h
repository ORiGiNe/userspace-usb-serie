#ifndef I2C_H
#define I2C_H

#include "DriverCapteur.h"

class I2C : public DriverCapteur
{
	public:
		I2C(int address);
		int get();
	private:
		int address;
};
#endif /*I2C_H*/
