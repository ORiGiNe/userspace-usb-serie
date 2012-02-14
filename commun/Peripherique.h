#ifndef OrIgInE_Device
#define OrIgInE_Device
#include "AbstractGaop.h"
#include "Commande.h"
/* Cette classe est une classe generique de peripherique tout peripherique
 * Le code de ce bidule sera different cote arduino et cote pc
 */

#ifdef IAmNotOnThePandaBoard
	extern "C" void __cxa_pure_virtual();
	#include <WProgram.h> //to have HIGH, LOW, digitalWrite, digitalRead, Serial.*, ...
#endif

class Peripherique
{
	public:
		Peripherique(octet odid);
		#ifndef IAmNotOnThePandaBoard
			virtual ~Peripherique();
		#else
		 	~Peripherique(); //a virtual destructor requires the delete operator which in his turn requires stdlibc++ wich is not supported by avr-g++... (http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?action=print;num=1209026667)
		#endif
		octet getOdid();
		void associe(AbstractGaop* g); //dit au peripherique d'uitliser l'objet gaop g
		
        virtual bool test(); //test le fonctionnement. Renvoie faux si ne marche pas
    	virtual void Receive(Commande&) = 0; //peripherique recoit des donnees
	protected:
		octet odid; //Numero unique identifiant le peripherique
		AbstractGaop* g; //gaop
};

class AssocPeriphOdid //associe un odid a un peripherique
{
	public:
		AssocPeriphOdid();
		~AssocPeriphOdid();
		void add(Peripherique&);
		void rm(octet odid); //desactive le peripherique odid
		Peripherique* operator[](int n); //nieme element (pour tout parcourrir) (constant)
		int getNbDevices(); //renvoie le nombre de devices
		Peripherique* getbyodid(octet odid); //recherche par identifiant (lineaire)
	private:
		Peripherique **t;
		int taille;
};

#endif /* OrIgInE_Device */

