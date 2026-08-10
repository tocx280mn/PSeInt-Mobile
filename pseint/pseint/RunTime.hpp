#ifndef RUNTIME_HPP
#define RUNTIME_HPP
#include "ErrorHandler.hpp"
#include "Programa.hpp"
#include "FuncsManager.hpp"
#include "new_memoria.h"

struct RunTime {
	ErrorHandler err;
	Programa prog;

//	Memoria *mem; (or Stack?)
	
	FuncsManager funcs;

};

#endif

