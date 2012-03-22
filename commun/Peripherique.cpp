#include "Peripherique.h"
#if IAmNotOnThePandaBoard
  void __cxa_pure_virtual() { };
#endif

Peripherique::Peripherique(int odid) 
{ 
	this->odid = odid; 
	this->g = NULL;
}

int Peripherique::getOdid() { return odid; }

void Peripherique::associe(AbstractGaop* g) { this->g = g; } //dit au peripherique d'utiliser l'objet gaop g

bool Peripherique::test() { return true; } //test le fonctionnenment du peripherique, et renvoie faux si il ne marche pas

bool Peripherique::operation()
{
	if (g == NULL) return false;
	bool ret = g->Send(cmd, odid);
	return ret;
}

//void Peripherique::Receive(Commande& c){  }

AssocPeriphOdid::AssocPeriphOdid()
{
	t = NULL;
	taille = 0;
}

AssocPeriphOdid::~AssocPeriphOdid()
{
	if (t) free(t);
}

void AssocPeriphOdid::add(Peripherique *p)
{
	taille++;
	t = (Peripherique**)realloc(t, taille*sizeof(Peripherique*));
	t[taille-1] = p;
}

Peripherique* AssocPeriphOdid::operator[](int n)
{
	if (n >= 0 &&  n < taille) return t[n];
	else return NULL;
}

Peripherique* AssocPeriphOdid::getbyodid(int odid)
{
	for (int i = 0; i < taille; i++)
	{
		if (t[i]->getOdid() == odid) return t[i];
	}
	return NULL;
}

void AssocPeriphOdid::rm(int odid)
{
	for (int i = 0; i < taille; i++)
	{
		if (t[i]->getOdid() == odid) {t[i] = NULL; break;}
	}
}

int AssocPeriphOdid::getNbDevices()
{
	return taille;
}
