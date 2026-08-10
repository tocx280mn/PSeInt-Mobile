#ifndef DEFINES_H
#define DEFINES_H
#include <string>
#include <vector>
#include "Instruccion.hpp"

// tipos auxiliares
using t_proceso = std::vector<Instruccion>;
using t_proceso_it = t_proceso::iterator;
//using t_programa = std::vector<t_proceso>;
//using t_programa_it = t_programa::iterator;
using t_output = std::vector<std::string>;
using t_output_it = t_output::iterator;
using t_arglist = std::vector<std::string>;
using t_arglist_it = t_arglist::iterator;


//#ifndef _USE_COUT
#define insertar(prog,text) prog.insert((prog).end(),text)
#define insertar_out(prog,out) { t_output_it it=(out).begin(); while (it!=(out).end()) { (prog).insert((prog).end(),*it); ++it; } }
#define mem_iterator map<string,tipo_var>::iterator
//#else
//#define insertar(prog,text) cerr<<text<<endl
//#endif

#endif
