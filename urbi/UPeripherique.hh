#ifndef UPERIPHERIQUE_HH 
#define UPERIPHERIQUE_HH 

#include "Peripherique.h"
#include <iostream>

class UPeripherique
{
	public:	
		UPeripherique();
		virtual Peripherique* getPeripherique() = 0;
}; 
#endif // ! UPERIPHERIQUE_HH 
