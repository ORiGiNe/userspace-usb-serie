#ifndef TRIGGERECHO
#define TRIGGERECHO

#include "DriverCapteur.h"

class TriggerEcho : public DriverCapteur
{
	public:
		TriggerEcho(int trigger, int echo);
		//TriggerEcho(int trigger_echo); TODO
		int get();
	private:
		int trigger;
		int echo;
};
#endif /*TRIGGERECHO*/
