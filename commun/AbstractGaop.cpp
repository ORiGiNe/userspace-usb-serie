#include "AbstractGaop.h"

AbstractGaop::AbstractGaop()
{
	this->prochain = 0;
	this->appel = 0;
	this->flags = 0;
	this->frames_recues = 0;
	this->frames_envoyees = 0;
}
