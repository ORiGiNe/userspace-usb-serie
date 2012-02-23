#ifndef UGAOP_HH 
#define UGAOP_HH 

// Include the UObject declarations. 
#include <urbi/uobject.hh> 

#include <iostream>

// include custom lib
#include "GaopProtocol.h"

// A UObject wrapping a custom object. 
class UGaop : public urbi::UObject
{ 
	public:
		// C++ contructor. 
		// @param name  name given to the instance. 
		UGaop(const std::string& name);
	        ~UGaop();	

		// Urbi constructor. 
		int init(const char* device); 

		// initialise g avec tblodid
		void initialise();

		// ajoute un périphérique à gérer
		void add(Peripherique* p);

		Gaop* getGaop();

	protected:
		Gaop* g;
		AssocPeriphOdid* tblassoc;
}; 
#endif // ! UGAOP_HH 
