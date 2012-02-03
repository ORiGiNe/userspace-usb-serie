#ifndef OrIgInE_Device
#define OrIgInE_Device
#include "GaopProtocol.h"
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
		Peripherique(const Peripherique&);
		#ifndef IAmNotOnThePandaBoard
			virtual ~Peripherique();
		#else
		 	~Peripherique(); //a virtual destructor requires the delete operator which in his turn requires stdlibc++ wich is not supported by avr-g++... (http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?action=print;num=1209026667)
		#endif
		int getOdid();
		void associe(Gaop &g); //dit au peripherique d'uitliser l'objet gaop g
		
        virtual bool test() = 0; //test le fonctionnement. Renvoie faux si ne marche pas
    	//virtual void Send(); //peripherique demande l'envoie sur l'usb
    	virtual void Receive(Commande&) = 0; //peripherique recoit des donnees
	protected:
		octet odid; //Numero unique identifiant le peripherique
		Gaop *g;
};

#endif /* OrIgInE_Device */
