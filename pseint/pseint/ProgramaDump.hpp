#ifndef PROGRAMADUMP_HPP
#define PROGRAMADUMP_HPP
#include "Programa.hpp"

Programa LoadPrograma(const std::string &fname);

bool SavePrograma(const std::string &fname, /*const*/ Programa &p);

#endif

