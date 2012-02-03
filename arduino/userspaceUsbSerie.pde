#include "Commande.h"
#include "GaopProtocol.h"
#include "Led.h"

Commande commandes;
//Warning : on ne peut pas faire de new ou de delete (mais on peut faire du malloc), ni user de la STL
Gaop brain;
Led led1(6);
Led led2(9);
#define NB_DEVICES 2
Peripherique* peripheriques[NB_DEVICES];
short int tables_des_odids[NB_DEVICES];

octet odid;

void setup()
{
	/**/pinMode(13, OUTPUT);	
	peripheriques[0] = &led1; 
	tables_des_odids[0] = (led1.test()) ? led1.getOdid() : -1;
	peripheriques[1] = &led2;
	tables_des_odids[1] = (led2.test()) ? led2.getOdid() : -1;
	brain.initialise(tables_des_odids, NB_DEVICES); //Serial.begin(115200);
	if (tables_des_odids[0] >= 0) led1.associe(brain);
	if (tables_des_odids[1] >= 0) led2.associe(brain);
}

void loop()
{
	if(brain.Receive(commandes, odid))
	{
		for (int i = 0; i < NB_DEVICES; i++)
		{
			if (tables_des_odids[i] == odid)
			{
				peripheriques[i]->Receive(commandes);
			}
		}
		while (commandes.getNbCommandes() > 0)
		{
			commandes.remove();
		} 
	}
}

