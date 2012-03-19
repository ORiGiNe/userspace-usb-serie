#ifndef COMMANDEFORGAOP
#define COMMANDEFORGAOP

/* On utilise les librairies C car les opertateurs new et delete, ainsi que la
 * STL ne sont pas supportés par l'arduino */ 
#include <stdlib.h>
#include "Config.h"

typedef unsigned char octet;
class Commande
{
	public:
		Commande();
#if IAmNotOnThePandaBoard
		~Commande();
#else 
		virtual ~Commande();
#endif
		
		short int& operator[](unsigned int i);
		//taille de la ieme commande (0 si non dispo)
		int getTaille() const;
		
	private:
		short int *tbl;
		unsigned int taille;
};

#endif
