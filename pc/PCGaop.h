#ifndef GAOPPROTOCOL
#define GAOPPROTOCOL

#include "AbstractGaop.h"
#include "Config.h"

#include <pthread.h>	/*pthread_* */

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

    private:
				/*!
				 *	File descriptor du slave
				 */
        int device;
				/*!
				 *	Le thread
				 */
        pthread_t fils;
        void **pthreadarg;
				/*!
				 * Historique des trames pour les renvoyées en cas de problèmes de communication
				 */
				Trame * trame_envoyees;

				/*!
				 * 	Sauvegarde de la trame pour historique
				 *	@param buf : Buffer contenant la trame ayant le format attendu
				 */
				bool save_trame(octet* buf);

				/*!
				 *	Construction d'une trame à partir de la structure gérant l'historique
				 */
				octet* build_trame_from_ack(Commande& cmd, octet seq);
};

#endif /*GAOPPROTOCOL */
