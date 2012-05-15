#ifndef GAOPPROTOCOL
#define GAOPPROTOCOL

#include "AbstractGaop.h"

#include <pthread.h>	/*pthread_* */
#include <string>

/*!
 *	\class Gaop
 *	\brief GAOP is An ORiGiNe protocole
 *	GAOP coté PC
 */
class PCGaop : public AbstractGaop
{
	public:
		PCGaop(const char *device);

		~PCGaop();

		void initialise(AssocPeriphOdid*);

		bool send(Commande &c, octet odid);

		bool receive(AssocPeriphOdid&);

	protected:

		void trame_to_string(octet *trame, int taille);

	private:
		int device; //file descriptor (open function)
		pthread_t fils; //pour le fork de Receive();
		void **pthreadarg;
};

#endif /*GAOPPROTOCOL */
