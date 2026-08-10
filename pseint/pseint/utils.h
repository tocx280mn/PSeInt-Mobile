#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "new_memoria.h"
#include "Programa.hpp"
#include "global.h"
#include <memory>

class RunTime;

void show_user_info(std::string msg);
void show_user_info(std::string msg1, int num, std::string msg2);

// ***************** Control de Errores y Depuración **********************

// ------------------------------------------------------------
//    Informa un error en tiempo de ejecucion
// ------------------------------------------------------------
void WarnError_impl(int num,std::string s, bool runtime) ;
void ExeError_impl(int num,std::string s) ;
void ExeError_impl(int num,std::string s, bool use_syn_if_not_running);

// ------------------------------------------------------------
//    Informa un error de syntaxis antes de la ejecucion
// ------------------------------------------------------------
void SynError_impl(int num,std::string s, CodeLocation il) ;
void SynError_impl(int num,std::string s) ;

// ------------------------------------------------------------
//    Comprueba que sea un identificador correcto y unico
//    A diferencia del anterior, no tiene en cuenta las
//  funciones predefinidas.
// ------------------------------------------------------------
bool CheckVariable(RunTime &rt, std::string str, int errcode=-1) ;


// *********************** Funciones Auxiliares **************************

// ----------------------------------------------------------------------
//    Compara los comienzos de dos cadenas (case sensitive)
// ----------------------------------------------------------------------
//bool LeftCompare(string a, string b) ;

// ----------------------------------------------------------------------
//    Compara parte de una cadena con otra (case insensitve)
// ----------------------------------------------------------------------
//bool MidCompareNC(string a, string b, int from) ;

// ----------------------------------------------------------------------
//    Averigua el tipo de variable para un dato
// ----------------------------------------------------------------------
tipo_var GuestTipo(std::string str) ;

// ----------------------------------------------------------------------
//    Reemplaza una cadena por otra si es que se encuentra
// ----------------------------------------------------------------------
bool ReplaceIfFound(std::string &str, std::string str1, std::string str2,bool saltear_literales=false) ;

bool TooManyDigits(const std::string &s);

bool IsInteger(double d);


// **************************************************************************

// funciones auxiliares para psexport
inline int max(int a,int b);
inline int min(int a,int b);

std::string CutString(std::string s, int a, int b=0);

// determina si una letra puede ser parte de una palabra clave o identificador
bool parteDePalabra(char c) ;

// corrige diferencias entre la codificación que usa pseint (ascii pelado) y la de la consola de windows
void fixwincharset(std::string &s, bool reverse=false);


// "extrae" una palabra, una constante, o un operador, desde la pos p, no modifica la cadena, sino que avanza el indice p
std::string NextToken(const std::string &cadena, int &p);


struct FuncStrings { std::string ret_id, nombre, args; };
FuncStrings SepararCabeceraDeSubProceso(std::string cadena);
std::unique_ptr<Funcion> MakeFuncionForSubproceso(RunTime &rt, const std::string &cadena, bool es_proceso);
std::unique_ptr<Funcion> MakeFuncionForSubproceso(RunTime &rt, const FuncStrings &parts, bool es_proceso);


#endif

