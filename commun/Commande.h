#ifndef COMMANDE_H
#define COMMANDE_H

/* On utilise les librairies C car les opertateurs new et delete, ainsi que la
 * STL ne sont pas supportés par l'arduino */
#define TBL_MAX 100

class Commande
{
	public:
		Commande();
		
		short int& operator[](unsigned int i);
		//taille de la ieme commande (0 si non dispo)
		int getTaille() const;
		void setTaille(int taille);
		
	private:
		short int tbl[TBL_MAX];
		unsigned int taille;
};

#endif
