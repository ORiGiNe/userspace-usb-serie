#ifndef GAOPPROTOCOL
#define GAOPPROTOCOL

#include "AbstractGaop.h"
#include "Config.h"

#include <pthread.h>	/* pthread_* */
#include <sys/select.h> /* fd_set */

/*!
 *	\class Gaop
 *	\brief GAOP is An ORiGiNe protocole
 *	Protocole de communication entre un slave (arduino) et un master (arduino).
 */
class PCGaop : public AbstractGaop
{
	public:
		/*!
		 *	Constructeur du protocole GAOP coté PC
		 *	@param slave Chemin absolu du device special
		 * - Ouverture du device
		 * - Set des paramètres liés à la communication série
		 * - Création de la structure du thread qui bouclera en réception
		 */
		PCGaop(const char *slave);

		~PCGaop();

		/*!
		 *	\param Tableau de devices
		 *	\brief Initialisation de la communication avec le slave :
		 *	- Vérification de la présence du protocole sur celui-ci ;
		 *	- Binding des devices du slave avec ceux déclarés dans le tableau
		 *	L'algorithme est détaillé dans le code
		 */
		void initialise(AssocPeriphOdid*);

		/*!
		 *	\brief Méthode pour envoyer une trame
		 *	\param c Commande à envoyer
		 *	\param odid Identifiant du device auquel il faut envoyer la commande.
		 */
		bool send(Commande &c, octet odid);

		/*!
		 *	\brief Méthode pour recevoir une trame
		 *	Cette méthode tourne en arrière-plan et permet de gérer les trames en réception
		 */
		bool receive(AssocPeriphOdid&);

	private:
		/*!
		 *	File descriptor du slave
		 */
		int slave;
		/*!
		 *	Thread pour lancer la méthode receive du Gaop et ainsi recevoir les trames du slave
		 */
		pthread_t fils;
		void **pthreadarg; // XXX: vraiment nécessaire ?
		/*!
		 * Historique des trames pour les renvoyées en cas de problèmes de communication.
		 */
		Trame * trame_envoyees;

		/*!
		 * 	Sauvegarde de la trame pour l'historique
		 *	@param buf Buffer contenant la trame ayant le format attendu
		 */
		void save_trame_before_send(octet* buf);

		/*!
		 *	Construction d'une trame à l'aide de la structure gérant l'historique et d'un numéro de séquence
		 *	Ce numéro de séquence est récupéré de la trame de nack TODO : ou d'un timeout.
		 */
		octet* build_trame_from_seq(Commande& cmd, octet seq);

		/*!
		 *	Récupération du numéro de séquence de la dernière commande envoyée sans ack
		 */
		int get_last_cmd_without_ack();

		/*!
		 *	Lit une trame dans le file descriptor, en minisant les appels systèmes.
		 *	Cette méthode utilise la méthode AbstractGaop::read_trame
		 *	\param cmd Commande à remplir
		 *	\param trame Trame à remplir
		 * 	\return Nombre d'octets lus
		 */
		int read_trame_from_fd(octet* trame, Commande& cmd, octet &odid);

		fd_set fdr, fdw, fde;

		octet trames_envoyees;
		bool periph_busy;
};

#endif /*GAOPPROTOCOL */
