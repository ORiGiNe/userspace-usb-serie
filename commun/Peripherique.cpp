#include "Peripherique.h"

#ifdef IAmNotOnThePandaBoard
  void __cxa_pure_virtual() { };
#endif

Peripherique::Peripherique(octet odid, Gaop &g) { this->odid = odid; this->g =&g; }
Peripherique::Peripherique(const Peripherique& p) { this->odid = p.odid; this->g = p.g; }
Peripherique::~Peripherique() { }

int Peripherique::getOdid() { return odid; }


//bool Peripherique::test() { return true; } //test le fonctionnenment du peripherique, et renvoie faux si il ne marche pas

//void Peripherique::Receive(Commande& c){  }
