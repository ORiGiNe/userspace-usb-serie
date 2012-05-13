#ifndef CAPTEUR_H
#define CAPTEUR_H

#include "Peripherique.h"
/*
notes:
on ne recupere qu'un octet (cf Receive), cette methode Receive est appelée par le thread si une trame contient l'odid correspondant
*/
class Capteur : public Peripherique
{
	public:
		Capteur(octet odid);
		int get();  //renvoie -32768 si timeout expirer
		void receive(Commande&); //traitement de la réponse au get
	private:
		bool reponse; //vrai si il y a eu une reponse a la requete XXX:pour le multi-thread 
};

#endif /*GAOPCAPTEUR*/
