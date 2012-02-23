#include "ULed.hh"

// Register the UObject in the Urbi world. 
UStart(ULed); 
 

ULed::ULed(const std::string& name) : urbi::UObject(name) {
	// Register the Urbi constructor.  
	// This is the only mandatory part of the C++ constructor.
	UBindFunction(ULed, init); 
}

/*ULed::~ULed() {
	 led;
}*/
 
int ULed::init(int odid, UObject* g) {
	// Bind the functions, i.e., declare them to the Urbi world
	// LOCK_INSTANCE Prevent parallel call to any function of this object.. 
	UBindThreadedFunction(ULed, allume, urbi::LOCK_INSTANCE);
	UBindThreadedFunction(ULed, eteint, urbi::LOCK_INSTANCE);
	
	led = new Led (0,6);
	UGaop* ugaop = dynamic_cast<UGaop*>(g);
	ugaop->add(led);
	return 0; 
}

void ULed::allume() {
	std::cout << "coucou" << std::endl;
	led->allume();
}

void ULed::eteint() {
	led->eteint();
}
