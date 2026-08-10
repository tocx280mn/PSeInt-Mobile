#ifndef EXPORT_EXP_H
#define EXPORT_EXP_H

#include<iostream>
#include<iomanip>
#include<stack>
#include "new_memoria.h"
#include "RunTime.hpp"
#include "strFuncs.hpp"

class ExpIntrucciones;

extern bool input_base_zero_arrays;
extern bool output_base_zero_arrays;

// conversion de expresiones
std::string modificarConstante(std::string s,int diff);

std::string buscarOperando(const std::string &exp, int comienzo, int direccion);

std::string colocarParentesis(const std::string &exp);

std::string sumarOrestarUno(std::string exp, bool sumar);

std::string expresion(RunTime &rt, std::string exp, tipo_var &tipo);

std::string expresion(RunTime &rt, std::string exp);

std::string invert_expresion(std::string expr);

#endif
