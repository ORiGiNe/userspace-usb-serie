#ifndef ABSTRACTGAOP
#define ABSTRACTGAOP
/*!
 *	\file AbstractGaop.h
 *	\brief Déclaration de l'abstract Gaop : header commun
 */

#include "Config.h"
#include "Commande.h"

// Forward declaration
class AssocPeriphOdid;

/*!
 *	\class AbstractGaop
 *	\brief Classe commune pour l'arduino et et le pc
 * 	On définit ici les attributs et méthodes communes du Gaop
 */
class AbstractGaop
{
	public:
		/*!
		 * Constructeur pour les attributs communs
		 */
		AbstractGaop();

		/*!
		 * 	\brief Initialisation du GAOP
		 * 	Le protocole de communication au démarrage est le suivant :
		 * 	PC => slave : je suis démarré, tu es prêt?
		 * 	PC <= slave : Oui
		 * 	PC => slave : Combien as-tu de devices ?
		 *	PC => slave : j’en ai X
		 *	for i in 0..X
		 *		PC => slave : donne moi le type et l'ODID du device i
		 *		PC <= slave : tiens
		 *		PC => slave : je le connais, active le et test son fonctionnement.
		 *		PC <= slave : ça marche.
		 *		PC instancie un device du type correspondant et associe le driver de slave
		 *	end
		 */

		/* Coté PC:
		 * PC : '>' Je suis demare. Tu es pret
		 * '?' : Tu as combien de devices ?
		 * 'i' : donne moi l'odid du device i
		 * 't' : test son fonctionnement
		 * ARDUINO:
		 * 'y' : oui
		 * 'n' : non
		 * <other> :  other data
		 */

		virtual void initialise(AssocPeriphOdid*) = 0;

		/*!
		 *	\brief Envoi de donnée
		 *	@param &c : Commande a envoyer
		 *	@param odid : ODID auquel la commande est envoyee
		 *	@return : False en cas d'echec d'envoi
		 */
		virtual bool send(Commande &c, octet odid) = 0;

		/*!
		 * 	\brief : Réception de commande
		 *	@return : False en cas d'echec de réception
		 * Cette méthode tourne en boucle, elle permet de recevoir les données.
		 * A la réception des données, celui-ci envoi au bon device (l'odid est dans la commande) via sa méthode receive
		 */
		virtual bool receive() = 0;

	protected:
		/*!
		 * Création du checksum
		 * \brief Creer le checksum d'une trame
		 * @desc Somme XOR de trame[0] à trame[taille-3]
		 * @param trame Trame sur laquelle faire le calcul
		 * @param taille Taille totale de la trame
		 * @return Renvoi le résultat du X0R 
		 */
		octet create_checksum( octet *trame, int taille); 

		/*!
		 * Construction de la trame
		 * trame =>
		 *	D = 1o : constante de debut
		 *	seq = 1o : n° sequence => relation d'ordre
		 *	t = 1o : taille data
		 *	odid = 1o : odid du periph
		 *	data = t*1o
		 *	CS = 1o : checksum == checkxor
		 *	F = 1o : constante de fin
		 *  
		 *  \brief prend les donnees data et les encapsules dans trames. 
		 *  Calcul aussi le checksum, et le met au bon endroit
		 *  @param data Donnees a envoyes
		 *  @param trame : trame a construire. doit etre suffisament grande pour
		 *  acceuilir les encapsulations et les donnees.
		 *  @param odid : identifiant du peripherique emetteur
		 *  @return : taille de la trame
		 */
		int create_trame(octet *trame, Commande data, octet odid); 

		/*!
		 *	Récupère la commande de la trame
		 *	@pre La trame doit être valide
		 *	@param trame La trame dans laquelle on récupère la commande
		 *	@return La commande
		 */
		Commande get_commande_from_trame(octet* trame);

		/*!
		 *	Récupère l'odid de la trame
		 *	@pre La trame doit être valide
		 *	@param La trame dans laquelle on récupère l'odid
		 *	@return L'odid
		 */
		octet get_odid_from_trame(octet* trame);

		/*!
		 * Extrait les informations de la trame
		 * @pre La trame doit être valide
		 * @param trame Trame lue 
		 * @param data Les informations ou commandes destinées au device précisé par l'ODID, ou
		 * dans le cas d'un ODID spécial, destiné aux traitements du protocole.
		 * @param odid L'ODID auquel la commande est destinée.
		 */
		void get_data_from_trame(octet *trame, Commande &data, octet &odid);

		/*!
		 *	Vide une trame en mettant tous les bytes à 0
		 */
		//void reset_trame(octet *trame);

		/*!
		 *	Vérifie une trame
		 *	\return true si la trame est bonne, false sinon
		 */
		bool verify_trame(octet *trame);

    AssocPeriphOdid *tblassoc;

		octet flags; //GAOPSPE, GAOPBLK, GAOPDBK, GAOPSND
};

#endif /*ABSTRACTGAOP */
