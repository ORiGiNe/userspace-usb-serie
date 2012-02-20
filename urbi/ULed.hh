#ifndef UTEST_HH 
#define UTEST_HH 

// Include the UObject declarations. 
#include <urbi/uobject.hh> 

#include <iostream>

// include custom lib
#include "Led.h"
#include "UGaop.hh"
#include "UPeripherique.hh"

// A UObject wrapping a custom object. 
class ULed : public urbi::UObject, public UPeripherique
{ 
	public: 
		// C++ contructor. 
		// @param name  name given to the instance. 
		ULed(const std::string& name);
	        //~ULed();	

		// Urbi constructor. 
		int init(int odid, UObject* g); 

		void allume();
		void eteint();

		Peripherique* getPeripherique();

	protected:
		Led* led; 
}; 
#endif // ! UTEST_HH 
