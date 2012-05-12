#ifndef ABSTRACTGAOP
#define ABSTRACTGAOP
/*!
 *	\file AbstractGaop.h
 *	\brief Déclaration de l'abstract Gaop : header commun
 */

#include "Config.h"
#include "Commande.h"

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
		virtual bool receive(AssocPeriphOdid&) = 0;

	protected:
		/*!
		 * Création du checksum
		 * \brief : cree le checksum d'une trame
		 * @desc : somme XOR sur toute la trame
		 * @param trame : trame sur laquelle faire le calcul
		 * @param taille : taille total de la trame
		 * @return : renvoie le resultat du xor (sur 1 octet)
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
		 *  @param data : donnees a envoyes
		 *  @param trame : trame a construire. doit etre suffisament grande pour
		 *  acceuilir les encapsulations et les donnees.
		 *  @param odid : identifiant du peripherique emetteur
		 *  @return : taille de la trame
		 */
		 int create_trame(octet *trame, commande &data, octet  odid); 


		/*!
		 * Lecture de la trame
		 * \desc Stocke dans data la trames lues
		 * @param trame : Ce qui est effectivement recue
		 * @param taille : taille de la trame
		 * @param data : Si la trame est correcte, cette objet contiendra les
		 * donnes recues. Dans le cas contraire, rien ne sera mis dans cet objet
		 * et la fonction returnera la valeur fausse.
		 * @param odid : contiendra l'identifiant du peripherique a qui les
		 * donnees s'addressent.
		 * @return : renvoie faux si la trames et incorrect, vrai sinon
		 */
		 bool read_trame(octet *trame, int taille, commande &data, octet &odid);
		
		octet prochain; //prochain numero disponible (% 256)
		octet appel; //candidat appele
		octet flags; //GAOPSPE, GAOPBLK, GAOPDBK, GAOPSND
		octet frames_envoyees;
		octet frames_recues;
};

#endif /*ABSTRACTGAOP */
