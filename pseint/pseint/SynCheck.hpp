#ifndef SYNCHECK_HPP
#define SYNCHECK_HPP
#include <string>
#include <vector>
#include "Instruccion.hpp"

class RunTime;

bool SynCheck(RunTime &rt);

// estas dos funciones son auxiliares de SynCheck
void Instrucciones(RunTime &rt);
void InformUnclosedLoops(RunTime &rt, std::vector<int> &bucles);

// estas tres funciones son auxiliares de Run y de ParseInspection
void Operadores(RunTime &rt, const int &x, std::string &cadena, InstructionType instruction_type);
std::pair<std::string,bool> Normalizar(std::string &cadena);
void Condiciones(RunTime &rt, std::string &cadena);

bool ParseInspection(RunTime &rt, std::string &exp);
bool SirveParaReferencia(RunTime &rt, const std::string &s);

#endif

