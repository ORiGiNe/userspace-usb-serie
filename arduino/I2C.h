#ifndef I2C
#define I2C

#include "DriverCapteur.h"

class I2c : public DriverCapteur
{
	public:
		I2c(int address);
		int get();
	private:
		int address;
};
#endif /*I2C*/
