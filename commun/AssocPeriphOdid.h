#ifndef ASSOCPERIPHODID_H
#define ASSOCPERIPHODID_H

#include "Peripherique.h"

class AssocPeriphOdid //associe un odid a un peripherique
{
	public:
		AssocPeriphOdid();
		~AssocPeriphOdid();

		void add(Peripherique*);
		void rm(octet odid); //desactive le peripherique odid
		Peripherique* operator[](int n); //nieme element (pour tout parcourrir) (constant)
		int getNbDevices(); //renvoie le nombre de devices
		Peripherique* getByODID(octet odid); //recherche par identifiant (lineaire)

	private:
		Peripherique **t;
		int taille;
};

#endif // ASSOCPERIPHODID_H
