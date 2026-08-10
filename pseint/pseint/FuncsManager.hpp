#ifndef FUNCSMANAGER_HPP
#define FUNCSMANAGER_HPP
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "new_memoria.h"
#include "DataValue.h"
#include "debug.h"

enum PASAJE { PP_VALOR, PP_REFERENCIA, PP_DESCONOCIDO };

class ErrorHandler; 
using FunctionPtr = DataValue (*)(ErrorHandler &err_handler, DataValue *args);

struct Funcion {
	std::string id; // function name (same as the key when used in FuncsManager's maps)
//	int cant_args; // arguments count... deleted for being redundant... vectors sizes are cant_args+1
	std::vector<PASAJE> pasajes; // tipo de pasaje de parametros (solo para las definidas por el usuario)
	std::vector<std::string> nombres; // nombres de los argumentos, la pos 0 es para el valor de retorno (solo para las definidas por el usuario, es "" si no retorna nada)
	std::vector<tipo_var> tipos; // la pos 0 es para el tipo que retorna
	const tipo_var &GetTipo(int i) const { return tipos[i]; } // para acceder a los tipos desde punteros const (0 es el de retorno)
	const PASAJE &GetPasaje(int i) const { return pasajes[i+1]; } // 0 es el 1er arg
	FunctionPtr func = nullptr; // NULL si es de las definidas por el usuario como subproceso
	int line_start = -1; // linea del pseudocodigo parseado donde comienza la funcion (solo para las definidas por el usuario)
	int userline_start = -1, userline_end = -1; // linea del pseudocodigo original donde empieza y termina la funcion (para pasarsela a la lista de variables del editor)
	std::unique_ptr<Memoria> memoria; // instancia de la clase memoria que se usa para el analisis sintático
	int GetArgsCount() const { return tipos.size()-1; }
	
	/// constructor for user defined functions
	Funcion(int line) : line_start(line) { AddArg(""); }
	void AddArg(std::string arg, PASAJE por=PP_DESCONOCIDO) { nombres.push_back(arg); tipos.push_back(vt_desconocido); pasajes.push_back(por); }
	void SetLastPasaje(PASAJE por) { pasajes[pasajes.size()-1]=por; } // para modificar el tipo de pasaje del último argumento insertado con AddArg
	
	/// constructor for language predefined functions
	Funcion(tipo_var tipo_ret, FunctionPtr af) : func(af) { 
		tipos.resize(1); tipos[0]=tipo_ret;
		pasajes.resize(1); pasajes[0]=PP_DESCONOCIDO;
	}
	/// constructor for language predefined functions
	Funcion(tipo_var tipo_ret, FunctionPtr af, tipo_var tipo_arg_1) : func(af) { 
		tipos.resize(2); tipos[0]=tipo_ret; tipos[1]=tipo_arg_1;
		pasajes.resize(2); pasajes[0]=pasajes[1]=PP_DESCONOCIDO;
	}
	/// constructor for language predefined functions
	Funcion(tipo_var tipo_ret, FunctionPtr af, tipo_var tipo_arg_1, tipo_var tipo_arg_2) : func(af) { 
		tipos.resize(3); tipos[0]=tipo_ret; tipos[1]=tipo_arg_1; tipos[2]=tipo_arg_2;
		pasajes.resize(3); pasajes[0]=pasajes[1]=pasajes[2]=PP_DESCONOCIDO;
	}
	/// constructor for language predefined functions
	Funcion(tipo_var tipo_ret, FunctionPtr af, tipo_var tipo_arg_1, tipo_var tipo_arg_2, tipo_var tipo_arg_3) : func(af) { 
		tipos.resize(4); tipos[0]=tipo_ret; tipos[1]=tipo_arg_1; tipos[2]=tipo_arg_2; tipos[3]=tipo_arg_3;
		pasajes.resize(4); pasajes[0]=pasajes[1]=pasajes[2]=pasajes[3]=PP_DESCONOCIDO;
	}
	
	Funcion(const Funcion &) = delete;
};

/**
* FuncsManager
*
* Clarification about terminology:
*    "sub" refers to a user's function or algoritms (declared withing the pseudocode)
*    "predef" refers to a languaje predefined function (such as cos, tan, log...)
*    "function" refers to either one, is used when distintion doesnt matter
**/

class FuncsManager {
public:
	using FuncsContainer = std::map<std::string,std::unique_ptr<Funcion>>;
	
	void LoadPredefs();
	void AddSub(std::unique_ptr<Funcion> &&func);
	Funcion *GetEditableSub(const std::string &name);
	
	bool HaveMain() const;
	void SetMain(const std::string &name);
	std::string GetMainName() const;
	const Funcion *GetMainFunc() const;
	
	bool IsSub(const std::string &name) const;
	bool IsPredef(const std::string &name) const;
	bool IsFunction(const std::string &name) const;
	const Funcion *GetSub(const std::string &name) const;
	const Funcion *GetFunction(const std::string &name, bool must_exists=true) const;
	const FuncsContainer &GetAllSubs() const { return m_subs; }
	const FuncsContainer &GetAllPredefs() const { return m_predefs; }
	
	void UnloadSubprocesos(); ///< borra de la memoria todos los subprocesos definidos por el usuario
	void UnloadPredefs(); ///< borra de la memoria todas las funciones predefinidas (nunca es necesario, solo para depurar el rtsyntax con memcheck y evitar falsos positivos)

	~FuncsManager();
	
private:
	FuncsContainer m_predefs; ///< funciones predefinidas del lenguaje
	FuncsContainer m_subs; ///< funciones definidas por el usuario (proceso y subprocesos)
	std::string m_main_name; ///< nombre de la funcion que representa al proceso principal, se asigna en SynCheck y se usa en el main para saber desde donde ejecutar

};

#endif

