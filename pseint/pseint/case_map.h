#ifndef CASEMAP_H
#define CASEMAP_H
#include <map>
#include <string>

/// @todo: convertir en clase... y ver si es una exageracion recibir el runtime solo para los nombres de subs

class RunTime;

// guarda en los nombres originales de las variables (antes de normalizarlos)
// para usar en la salida (para diagramas de flujo mayormente), 
// NULL quiere decir que no se hace
extern std::map<std::string,std::string> *case_map; 

void CaseMapFill(const RunTime &rt, std::string &s);
//void CaseMapPurge();
void CaseMapApply(const RunTime &rt, std::string &s, bool and_fix_parentesis);
void InitCaseMap();

#ifndef _FOR_PSEXPORT
class Programa;
void CaseMapApply(const RunTime &rt, Programa &programa, bool for_export);
#endif

#endif

