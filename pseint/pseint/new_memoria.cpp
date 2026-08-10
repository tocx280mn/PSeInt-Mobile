#include "new_memoria.h"
#include "FuncsManager.hpp"

Memoria *memoria;
bool Memoria::EsArgumento (const std::string &nom) const {
	if (!funcion) return false;
	for(int i=1;i<=funcion->GetArgsCount();i++) 
		if (funcion->nombres[i]==nom) return true;
	return false;
}

