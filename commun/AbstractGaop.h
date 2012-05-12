#ifndef ABSTRACTGAOPPROTOCOL
#define ABSTRACTGAOPPROTOCOL
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
		
		virtual bool Send(Commande &c, unsigned short int odid) = 0; 		
	private:
		octet prochain; //prochain numero disponible (% 256)
		octet appel; //candidat appele
		octet flags; //GAOPSPE, GAOPBLK, GAOPDBK, GAOPSND
		octet frames_envoyees;
		octet frames_recues;

		
};

#endif /*ABSTRACTGAOPPROTOCOL*/
