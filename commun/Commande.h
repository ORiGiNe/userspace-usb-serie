#ifndef COMMANDE_H
#define COMMANDE_H

/* On utilise les librairies C car les opertateurs new et delete, ainsi que la
 * STL ne sont pas supportés par l'arduino */

class Commande
{
	public:
		Commande();
		virtual ~Commande();
		
		short int& operator[](unsigned int i);
		//taille de la ieme commande (0 si non dispo)
		int getTaille() const;
		
	private:
		short int *tbl;
		unsigned int taille;
};

#endif
