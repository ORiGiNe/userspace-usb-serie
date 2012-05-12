#ifndef ABSTRACTGAOP
#define ABSTRACTGAOP
/*!
 *	\file AbstractGaop.h
 *	\brief Déclaration de l'abstract Gaop : header commun
 */
#include "Config.h"
#include "Commande.h"

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

		virtual void initialise(AssocPeriphOdid*);

		/*!
		 *	\brief Envoi de donnée
		 *	@param &c : Commande a envoyer
		 *	@param odid : ODID auquel la commande est envoyee
		 *	@return : False en cas d'echec d'envoi 
		 */
		virtual bool Send(Commande &c, unsigned short int odid) = 0; 		
		
		/*!
		 * 	\brief : Réception de commande
		 *	@return : False en cas d'echec de réception
		 * Cette méthode tourne en boucle, elle permet de recevoir les données.
		 * A la réception des données, celui-ci envoi au bon device (l'odid est dans la commande) via sa méthode Receive (TODO:changer ce nom)
		 */
		virtual bool Receive(AssocPeriphOdid&);

	private:
		/*!
			* Construction du checksum
			*/

		/*!
		 * Vérification du checksum
		 */

		/*!
		 * Construction de la trame
		 */

		/*!
		 * Lecture de la trame
		 */
		
		octet prochain; //prochain numero disponible (% 256)
		octet appel; //candidat appele
		octet flags; //GAOPSPE, GAOPBLK, GAOPDBK, GAOPSND
		octet frames_envoyees;
		octet frames_recues;
};

#endif /*ABSTRACTGAOP */
