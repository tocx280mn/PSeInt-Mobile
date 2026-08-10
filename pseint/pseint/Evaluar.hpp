#ifndef EVALUAR_HPP
#define EVALUAR_HPP
#include "new_memoria.h"
#include "DataValue.h"

class Funcion;
class RunTime;

bool CheckDims(RunTime &rt, std::string &str); // recibe una cadena del tipo "lala(1,x)" y verifica que lala sea un arreglo y los indices sean correctos (la modifica y la vuelve a dejar como estaba, por eso no es const, pero a fines practicos si)
int BuscarComa(const std::string &expresion, int p1, int p2, char coma=','); // busca en [p1,p2], retorna -1 si no esta
int BuscarOperador(const std::string &expresion, int &p1, int &p2);
tipo_var DeterminarTipo(RunTime &rt, const std::string &expresion, int p1, int p2);
bool AplicarTipo(RunTime &rt, const std::string &expresion, int &p1, int &p2, tipo_var tipo);
DataValue EvaluarFuncion(RunTime &rt, const Funcion *func, const std::string &argumentos, const tipo_var &forced_tipo, bool for_expresion=true);
DataValue EvaluarFuncion(RunTime &rt, const std::string &func_name, const std::string &argumentos, const tipo_var &forced_tipo, bool for_expresion=true);
DataValue Evaluar(RunTime &rt, const std::string &expresion, int &p1, int &p2, const tipo_var &forced_tipo=vt_desconocido, const char *desc=nullptr);
DataValue Evaluar(RunTime &rt, std::string expresion, const tipo_var &forced_tipo=vt_desconocido);
DataValue EvaluarSC(RunTime &rt, std::string expresion, const tipo_var &forced_tipo=vt_desconocido, const char *desc=nullptr);
bool PalabraReservada(const std::string &str);

#endif

