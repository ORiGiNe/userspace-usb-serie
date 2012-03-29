#include "I2C.h"
#include <WProgram.h>
#include <Wire.h>
/*
	README:
	Ajouter Wire.h dans le programme principal pour l'installation
	via l'IDE arduino

	Le Wire.begin() ne devrait être appelé qu'une seule fois
	Il faudrait le mettre dans le main
*/

I2c::I2c(int address)
{
	Wire.begin();
	this->address = address;
}

int I2c::get()
{
  int result = 0;
  
  // Pour enregistrer la valeur en cm
  Wire.beginTransmission(this->address);
  Wire.send(0);
  Wire.send(0x51);
  Wire.endTransmission();
  delay(70);
  
  // Pour lire
  Wire.beginTransmission(this->address);
  Wire.send(2);
  Wire.endTransmission();
  
  Wire.requestFrom(this->address, 2);

  while (Wire.available() < 2 )   {
    // wait for result
  }

	// Décalage logique d'un bit
  result = Wire.receive() * 256;
  result += Wire.receive();
  
  return result;
}
