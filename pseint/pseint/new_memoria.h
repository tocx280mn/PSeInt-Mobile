#ifndef NEW_MEMORIA_H
#define NEW_MEMORIA_H
#include <cstddef>
#include <map>
#include <string>
#include <iostream>
#include <list>
#include <sstream>
#include <set>
#include "new_memoria_inter.h"
#include "DataValue.h"

class Memoria;

struct alias {
	std::string nom;
	Memoria *mem;
	alias():mem(NULL){}
	alias(std::string n, Memoria *m):nom(n),mem(m){}
};

// cppcheck-suppress noConstructor
class Funcion;

class Memoria {
	
	std::map<std::string,alias> var_alias; // lista de aliases, para el pasaje por referencia
	std::string alias_nom; Memoria *alias_mem;
	
	std::map<std::string,tipo_var> var_info;
	std::map<std::string,DataValue> var_value;
	void QuitarIndices(std::string &str) {
		int sz=str.size();
		for (int i=0;i<sz;i++)
			if (str[i]=='['||str[i]=='(') {
				str.erase(i);
				break;
			}
#ifdef _FOR_PSEXPORT
			else str[i]=toupper(str[i]);
#endif
		
	}
	bool EsAlias(const std::string &s) { // mira si la variable que recibe es un alias, setea it_alias
		auto it_alias=var_alias.find(s);
		if (it_alias==var_alias.end()) return false;
		alias_nom=it_alias->second.nom;
		alias_mem=it_alias->second.mem;
		return true;
	}
	bool EsAlias(const std::string &s, bool) { // mira si la expresion que recibe es un alias (puede tener dimensiones, las quita si es asi), setea it_alias
		std::string s2 = s; QuitarIndices(s2);
		auto it_alias=var_alias.find(s2);
		if (it_alias==var_alias.end()) return false;
		alias_nom=it_alias->second.nom+s.substr(s2.size());
		alias_mem=it_alias->second.mem;
		return true;
	}
	const Funcion *funcion; // scope
public:
	// cppcheck-suppress uninitMemberVar
	Memoria(const Funcion *parent):funcion(parent){} // guarda el puntero a su scope para usar en EsArgumento
	bool EsArgumento(const std::string &nom) const; // determina si un nombre dado es uno de los argumentos de la función actual o no
	void HardReset() {
		var_value.clear();
		var_info.clear();
	}
	void FakeReset() {
		for (auto &p : var_info)
			p.second.reset();
		var_value.clear();
	}
//	void Agregartipo_var(const string &nombre, const tipo_var &tipo=vt_desconocido) {
//		tipo_var &v = var_info[nombre];
//		v.set(tipo);
//	}
	void AgregarArreglo(std::string nombre, int *dims) {
#ifdef _FOR_PSEXPORT
		for(unsigned int i=0;i<nombre.size();i++) nombre[i]=toupper(nombre[i]);
#endif
		tipo_var &v = var_info[nombre];
		v.dims=dims;
	}
	
	void RedimensionarAux(std::string nombre, int *odims, int *ndims, int idim, int cur=0) {
		if (cur++==odims[0]) 
			var_value.erase(nombre+")");
		for (int i = cur==idim+1?ndims[cur]:0; i < odims[cur]; ++i)
			RedimensionarAux(nombre+(cur==1?"(":",")+std::to_string(i+1),odims,ndims,idim,cur);
	}
	
	void RedimensionarArreglo(std::string nombre, int *dims) {
#ifdef _FOR_PSEXPORT
		for(unsigned int i=0;i<nombre.size();i++) nombre[i]=toupper(nombre[i]);
#endif
		tipo_var &v = var_info[nombre];
		for(int i=0;i<dims[0];++i) {
			if (dims[i+1]<v.dims[i+1]) {
				RedimensionarAux(nombre,v.dims,dims,i);
				v.dims[i+1]=dims[i+1];
			}
		}
		_expects(v.dims and v.dims[0]==dims[0]);
		delete v.dims;
		v.dims = dims;
	}
	// esta version de definir tipo se usa en las definiciones implicitas
	void AgregarAlias(const std::string &nom_here, const std::string &nom_orig, Memoria *mem) {
		var_alias[nom_here]=alias(nom_orig,mem);
		var_info[nom_here]=mem->var_info[nom_orig];
		if (nom_orig.find('(')!=std::string::npos) var_info[nom_here].dims=NULL;
	}
	// esta version de definir tipo se usa en las definiciones implicitas
	bool DefinirTipo(std::string nombre, const tipo_var &tipo) {
#ifdef _FOR_PSEXPORT
		// esto es porque el parseo que hace la funcion expresion pasa todo a minuscula,
		// pero despues los i_* pueden llamar a DefinirTipo por su cuenta con la expresion 
		// ya parseada y pasada a minusculas
		for(unsigned int i=0;i<nombre.size();i++) nombre[i]=toupper(nombre[i]);
#endif
		QuitarIndices(nombre);
		if (EsAlias(nombre)) return alias_mem->DefinirTipo(alias_nom,tipo);
		tipo_var &vi=var_info[nombre];
//		vi.defined=true; // en las implicitas esto no va
		return vi.set(tipo,true);
	}
	// esta version de definir tipo se usa en las definiciones explictas
	void DefinirTipo(std::string nombre, const tipo_var &tipo, bool rounded) {
		QuitarIndices(nombre);
		if (EsAlias(nombre)) return alias_mem->DefinirTipo(alias_nom,tipo,rounded);
		tipo_var &vi=var_info[nombre];
		if (rounded) vi.rounded=true;
		vi.defined=true;
		vi.cb_car=tipo.cb_car;
		vi.cb_log=tipo.cb_log;
		vi.cb_num=tipo.cb_num;
	}
	void EscribirValor(const std::string &nombre, const DataValue &valor) {
		if (EsAlias(nombre,true)) return alias_mem->EscribirValor(alias_nom,valor);
		std::string nom = nombre; QuitarIndices(nom);
		tipo_var &vi = var_info[nom];
		vi.enabled=true;
		vi.used=true;
		if (vi.rounded) {
			/// @todo: ver si esto puede pasar, deberia controlarse antes y dar un error
			var_value[nombre].SetFromInt(valor.GetAsInt());
		}
		var_value[nombre].SetValue(valor);
	}
	const int *LeerDims(std::string nombre) {
		QuitarIndices(nombre);
		if (EsAlias(nombre)) {
			if (alias_nom.find('(')!=std::string::npos) // si el original era una arreglo, pero el alias apunta solo a una posicion
				return NULL;
			else
				return alias_mem->LeerDims(alias_nom);
		}
		return var_info[nombre].dims;
	}
	tipo_var LeerTipo(std::string nombre) {
		QuitarIndices(nombre);
		if (EsAlias(nombre)) return alias_mem->LeerTipo(alias_nom);
		tipo_var &vi = var_info[nombre];
		return vi;
	}
	bool Existe(std::string nombre) {
		QuitarIndices(nombre);
		if (EsAlias(nombre)) return alias_mem->Existe(alias_nom);
		auto it_info = var_info.find(nombre);
		if (it_info==var_info.end()) return false;
//		tipo_var &vi=it_info->second;
		return it_info->second.enabled;
	}
	bool HaSidoUsada(std::string nombre) {
		QuitarIndices(nombre);
		if (EsAlias(nombre)) return alias_mem->HaSidoUsada(alias_nom);
		auto it_info = var_info.find(nombre);
		if (it_info==var_info.end()) return false;
//		tipo_var &vi=it_info->second;
		return it_info->second.enabled && it_info->second.used;
	}
	bool EstaInicializada(const std::string &nombre) {
		std::string nom = nombre; QuitarIndices(nom);
		if (EsAlias(nombre,true)) return alias_mem->EstaInicializada(alias_nom);
		auto it_info = var_info.find(nom);
		if (it_info==var_info.end()) return false;
//		tipo_var &vi=it_info->second;
		return var_value.count(nombre);
	}
	bool EstaDefinida(std::string nombre) {
		QuitarIndices(nombre);
		if (EsAlias(nombre)) return alias_mem->EstaDefinida(alias_nom);
		auto it_info = var_info.find(nombre);
		if (it_info==var_info.end()) return false;
//		tipo_var &vi=it_info->second;
		return  var_info[nombre].defined;
	}
	void Desinicializar(std::string nombre) {
		QuitarIndices(nombre);
		if (EsAlias(nombre)) return alias_mem->Desinicializar(alias_nom);
		var_value.erase(nombre);
	}
	void SetearSoloLectura(std::string nombre, bool set) {
		QuitarIndices(nombre);
		if (EsAlias(nombre)) return alias_mem->SetearSoloLectura(alias_nom,set);
		var_info[nombre].read_only = set;
	}
	DataValue LeerValor(const std::string &nombre) {
		if (EsAlias(nombre,true)) return alias_mem->LeerValor(alias_nom);
		DataValue ret = var_value[nombre];
		if (ret.IsEmpty()) {
			std::string nom = nombre; QuitarIndices(nom);
			tipo_var &vi = var_info[nom];
			vi.used=true; // fuera del if no hace falta porque si ret!=0 es porque paso por EscribirValor, y entonces ya esta used=true
		}
		return ret;
	}
	DataValue Leer(const std::string &nombre) {
		DataValue res = LeerValor(nombre);
		res.type = LeerTipo(nombre);
		return res;
	}
	void ListVars(std::map<std::string,std::string> *case_map) { // para que el proceso de rt_syntax le pase a la gui la lista de variables
		for(auto &p : var_info) {
			if (p.first=="") continue; // cuando hay errores de sintaxis, pueden salir variables sin nombre
			std::cout << ( case_map ? (*case_map)[p.first] : p.first );
			if (p.second.dims && p.second.dims[0]>0) {
				std::cout << '[';
				for (int i=1; i<p.second.dims[0]; i++)
					std::cout << p.second.dims[i] << ',';
				std::cout << p.second.dims[p.second.dims[0]] << ']';
			}
			std::cout << ' ' << char(LV_BASE_CHAR+(
				                       (p.second.cb_log?LV_LOGICA:0)   +
				                       (p.second.cb_num?LV_NUMERICA:0) +
				                       (p.second.cb_car?LV_CARACTER:0) +
				                       (p.second.defined?LV_DEFINIDA:0)
				                    )) << std::endl;;
		}
		
	}
	
#ifdef _FOR_PSEXPORT
	std::map<std::string,tipo_var> &GetVarInfo() { return var_info; }
	void RemoveVar(std::string nombre) {
		for(unsigned int i=0;i<nombre.size();i++) nombre[i]=std::toupper(nombre[i]);
		if (var_info.find(nombre)!=var_info.end()) var_info.erase(var_info.find(nombre));
		if (var_value.find(nombre)!=var_value.end()) var_value.erase(var_value.find(nombre));
	}
#endif
	~Memoria() {
		for(auto &p :var_info)
			if (p.second.dims && var_alias.count(p.first)==0)
				delete [] p.second.dims;
	}
	
private:
	Memoria(const Memoria &); // forbidden
	Memoria &operator=(const Memoria &);  // forbidden
};

extern Memoria *memoria;

#endif

