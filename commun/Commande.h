#ifndef COMMANDEFORGAOP
#define COMMANDEFORGAOP

/* On utilise les librairies C car les opertateur new et delete, ainsi que la
 * STL ne sont pas supportés par l'arduino */ 
#include <stdlib.h>
#include <string.h>
#include "Config.h"

typedef unsigned char octet;
class Commande
{
	public:
		Commande();
		~Commande();
		
		//ajoute la commande cmd. si cmd == NULL, seul la taille de la commande et alloue
		void add(const octet* cmd, int taille);
		void add(int i); //envoie d'un entier
		
		//supprime la derniere
		void remove();
		
		//obtenir la commande numero i (NULL si non dispo)
		octet* get(int i);
		short int getint(int i); //si c'est un entier
		octet* operator[](int i);
		//taille de la ieme commande (0 si non dispo)
		int getTaille(int i) const;
		int getNbCommandes() const; //renvoie le nombre de commande
		
	private:
		octet **cmd;
		int nb_element;
		int *taille_des_cmd;
		int taille_de_la_frame;
};

#endif
