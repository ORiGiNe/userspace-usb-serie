#ifndef GAOPPROTOCOL
#define GAOPPROTOCOL

#include "Peripherique.h" 
#include "AbstractGaop.h"

#include <sys/stat.h>	/*open*/
#include <fcntl.h>		/*open*/
#include <pthread.h>	/*pthread_* */
#include <signal.h>		/*kill*/
#include <unistd.h>		/*read, write, close, fork*/
#include <termios.h>	/*tcgetattr, cfsetospeed, cfsetispeed, tcsetattr, tcdrain struct termios*/
#include <time.h>		/*clock_gettime (implique -lrt), nanosleep*/ 
#include <iostream>		/*cerr, cout, endl*/
#include <cstring>		/*strerr*/
#include <cerrno>		/*errno*/
#include <WProgram.h> //to have HIGH, LOW, digitalWrite, digitalRead, Serial.*, ...

/*!
 *	\class Gaop
 *	\brief GAOP is An ORiGiNe protocole
 *	GAOP coté PC	
 */
class Gaop : public AbstractGaop
{
	public:
		Gaop(const char *device); 

    ~Gaop();
		
		void initialise(AssocPeriphOdid*);

		bool Send(Commande &c, unsigned short int odid); 		
		
		bool Receive(AssocPeriphOdid&);
		
	private:
		int device; //file descriptor (open function)
		pthread_t fils; //pour le fork de Receive();
		void **pthreadarg;
};

#endif /*GAOPPROTOCOL */
