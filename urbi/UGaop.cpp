#include "UGaop.hh"
#include "UPeripherique.hh"

// Register the UObject in the Urbi world. 
UStart(UGaop); 
 

UGaop::UGaop(const std::string& name) : urbi::UObject(name) {
	// Register the Urbi constructor.  
	// This is the only mandatory part of the C++ constructor.
	UBindFunction(UGaop, init);
}

UGaop::~UGaop() {
	delete g;
	delete tblassoc;
}
 
int UGaop::init(const char* device) {
	// Bind the functions, i.e., declare them to the Urbi world
	UBindFunction(UGaop, initialise);
	UBindFunction(UGaop, add);
	
	g = new Gaop (device);
	return 0; 
}


void UGaop::initialise() {
	g->initialise(tblassoc);
}


void UGaop::add(UObject* u) {
	UPeripherique* p = dynamic_cast<UPeripherique*>(u);
	tblassoc->add(p->getPeripherique());
}

Gaop* UGaop::getGaop() {
	return g;
}
