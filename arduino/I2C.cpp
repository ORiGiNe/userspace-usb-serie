#include "I2C.h"
#include <Arduino.h>
#include <Wire.h>

/*
	README:
	Ajouter Wire.h dans le programme principal pour l'installation
	via l'IDE arduino

	Le Wire.begin() ne devrait être appelé qu'une seule fois
	Il faudrait le mettre dans le main
*/

I2C::I2C(int _address)
{
	Wire.begin();
	address = _address;
}

int I2C::get()
{
	int result = 0;

	// Pour enregistrer la valeur en cm
	Wire.beginTransmission(address);
	Wire.write(0);
	Wire.write(0x51);
	Wire.endTransmission();
	delay(70);

	// Pour lire
	Wire.beginTransmission(address);
	Wire.write(2);
	Wire.endTransmission();

	Wire.requestFrom(address, 2);

	while (Wire.available() < 2) {
		// wait for result
	}

	// Décalage logique d'un octet
	result = Wire.read() * 256;
	result += Wire.read();

	return result;
}
