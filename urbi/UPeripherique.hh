#ifndef UPERIPHERIQUE_HH 
#define UPERIPHERIQUE_HH 

#include "Peripherique.h"
#include <iostream>

// A UObject wrapping a custom object. 
class UPeripherique
{
	public:	
		virtual Peripherique* getPeripherique();
}; 
#endif // ! UPERIPHERIQUE_HH 
