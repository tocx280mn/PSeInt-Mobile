#ifndef DATAVALUE_H
#define DATAVALUE_H
#include <string>
#include <cstdlib>
#include <cstdio>
#include <variant>
#include "debug.h"

#define FALSO "FALSO"
#define VERDADERO "VERDADERO"

#if 1
// preferiría usar las implementaciones del else, basadas en from_chars y to_chars,
// pero en el SDK para mac que uso solo estan las versiones para enteros
// y en el gcc del mint (7.x) ni está el header
inline double StrToDbl(const std::string &s) {
	return atof(s.c_str());
}

inline std::string DblToStr(double d) {
	char buf[512]; // DBL_MAX ocupa 310 caracteres
	sprintf(buf,"%.50f",d);
	// eliminar los ceros que sobren y el punto si era entero
	int i=0; while (buf[i]!=0) i++;
	while (buf[--i]=='0'); // contar ceros de atras para adelante
	if (buf[i]=='.') i--; // si llegamos al '.' sacarlo tambien
	if (buf[0]=='-'&&buf[1]=='0'&&i==1) buf[i=0]='0'; // si quedo "-0" sacar el -
	buf[++i]='\0'; // aplicar el corte
	return buf;
}

#else
#include <charconv>

inline double StrToDbl(const std::string &s) {
	double d=0;
	auto p = s.data(); if (*p=='+') ++p;
	auto r1 = std::from_chars(p, s.data()+s.size(), d);
	return d;
}

inline std::string DblToStr(double d) {
	thread_local static char sout[24];
	auto [p,ec] = std::to_chars(sout,sout+sizeof(sout),d); *p='\0';
	if (ec!=std::errc()) { sout[0]='0'; p=sout+1; }
	*p = '\0';
	return sout;
}

#endif

// esta es la version para escribir en pantalla, no para las conversiones internas...
// por eso mantengo la implementacion "vieja", para controlar la cantidad de decimales
// y la notacion
inline std::string DblToStr(double d, bool low) {
	_expects(low);
	char buf[512]; // DBL_MAX ocupa 310 caracteres
	sprintf(buf,"%.10f",d); // version de baja precisión, se debería usar solo para mostrar
	// eliminar los ceros que sobren y el punto si era entero
	int i=0; while (buf[i]!=0) i++;
	while (buf[--i]=='0'); // contar ceros de atras para adelante
	if (buf[i]=='.') i--; // si llegamos al '.' sacarlo tambien
	if (buf[0]=='-'&&buf[1]=='0'&&i==1) buf[i=0]='0'; // si quedo "-0" sacar el -
	buf[++i]='\0'; // aplicar el corte
	return buf;
}

/// @todo: usar to_chars/from_chars aca tambien
// en este caso el cambio no es crítico, no hay problemas de redondeo cuando son enteros
inline int StrToInt(const std::string &s) {
	return atoi(s.c_str());
}

inline std::string IntToStr(int l) {
	char buf[4*sizeof(int)]; // para un long de 64bits, el más largo tiene 20 cifras y estaría reservando 32
	sprintf(buf,"%i",l);
	return buf;
}

inline std::string BoolToStr(bool b) {
	return b?"VERDADERO":"FALSO";
}

inline bool StrToBool(const std::string &s) {
	return s=="VERDADERO";
}


struct tipo_var {
	friend class Memoria;
	//private: // el cliente pseint debe acceder a travez de memoria->LeerDims para que si es alias lo corrija (pero psexport si necesita acceder directo)
	int *dims = nullptr; // dims[0] es la cantidad de dimensiones, dims[1...] son las dimensiones propiamente dichas
public:
	bool enabled = true; // para que queden registradas luego del primer parseo, pero actue como si no existieran
	bool cb_log = true, cb_num = true, cb_car = true; // si puede ser logica, numerica o caracter
	bool rounded = false; // para cuando se definen como enteras
	bool defined = false; // para saber si fueron definidas explicitamente (definir...)
	bool used = false; // para saber si fue usada, asignada, leida, algo que no sea dimensionada o definida explicitamente, lo setean Escribir y LeerValor
	bool read_only = false; // para saber si el usuario puede modificarla (ej, el contador de un para sería read-only para el usuario, solo lo modifica el para)
	tipo_var() {}
	tipo_var(bool l, bool n, bool c, bool r=false):cb_log(l),cb_num(n),cb_car(c),rounded(r) {}
	bool set(const tipo_var &v) {
		enabled=true;
		cb_log=cb_log&&v.cb_log;
		cb_num=cb_num&&v.cb_num;
		cb_car=cb_car&&v.cb_car;
		return (cb_car?1:0)+(cb_log?1:0)+(cb_num?1:0)!=0;
	}
	bool set(const tipo_var &v, bool) {
		enabled=true;
		bool error = ((cb_log&&v.cb_log)?1:0)+((cb_num&&v.cb_num)?1:0)+((cb_car&&v.cb_car)?1:0)==0;
		if (v.rounded) rounded=true;
		if (!error) {
			cb_log=cb_log&&v.cb_log;
			cb_num=cb_num&&v.cb_num;
			cb_car=cb_car&&v.cb_car;
			return true;
		} else
			return false;
	}
	bool is_known() const {
		return (cb_car?1:0)+(cb_log?1:0)+(cb_num?1:0)==1;
	}
	bool is_ok() const {
		return (cb_car?1:0)+(cb_log?1:0)+(cb_num?1:0)!=0;
	}
	bool operator==(const tipo_var &t) const {
		return cb_car==t.cb_car&&cb_num==t.cb_num&&cb_log==t.cb_log;
	}
	bool operator!=(const tipo_var &t) const {
		return cb_car!=t.cb_car||cb_num!=t.cb_num||cb_log!=t.cb_log;
	}
	bool can_be(const tipo_var &t) const {
		return (cb_car&&t.cb_car) || (cb_num&&t.cb_num) || (cb_log&&t.cb_log);
	}
	// cppcheck-suppress operatorEqVarError
	tipo_var &operator=(const tipo_var &t) {
		cb_log=t.cb_log;
		cb_num=t.cb_num;
		cb_car=t.cb_car;
		rounded=t.rounded;
		dims=t.dims;
		return *this;
	}
	void reset() { // para borrar la información que genera el analisis sintáctico antes de la ejecución y que no debe pasar a la ejecución
		read_only=defined=used=enabled=false;
		if (dims) { delete [] dims; dims=NULL; }
	}
};

extern tipo_var vt_error;
extern tipo_var vt_desconocido;
extern tipo_var vt_logica;
extern tipo_var vt_numerica;
extern tipo_var vt_caracter;
extern tipo_var vt_caracter_o_numerica;
extern tipo_var vt_caracter_o_logica;
extern tipo_var vt_numerica_entera;


struct DataValue {
	
	tipo_var type;
	using variant_t = std::variant<std::monostate,double,std::string,int,bool>;
	variant_t value; // monostate equivale a vacio
	
	DataValue() {}
	DataValue(tipo_var t) : type(t) {}
	DataValue(tipo_var t, int v) : type(t) { value = v; }
	DataValue(tipo_var t, double v) : type(t) { value = v; }
	DataValue(tipo_var t, bool v) : type(t) { value = v; }
	DataValue(tipo_var t, const std::string &v) : type(t) { value = v; }
	
	bool IsOk() const { return type!=vt_error; } // tipo
	bool IsError() const { return type==vt_error; } // tipo
	bool IsEmpty() const { return std::holds_alternative<std::monostate>(value); } // valor
	
	bool CanBeLogic() const  { return type.cb_log; }
	bool CanBeReal() const   { return type.cb_num; }
	bool CanBeString() const { return type.cb_car; }
	bool IsLogic() const  { return  type.cb_log && !type.cb_num && !type.cb_car; }
	bool IsReal() const   { return !type.cb_log &&  type.cb_num && !type.cb_car; }
	bool IsString() const { return !type.cb_log && !type.cb_num &&  type.cb_car; }
	
	// estos métodos se implementan abajo para poder definir un std::get alternativo para mac os
	bool GetAsBool() const;
	double GetAsReal() const;
	int GetAsInt() const;
	std::string GetAsString() const;
	std::string GetForUser() const;
	
	void SetFromString(const std::string &s) { _expects(type.cb_car); value = s; }
	void SetFromInt(int i) { _expects(type.cb_num); value = i; }
	void SetValue(const DataValue &other) { value = other.value; } /// @todo: ver quien usa esto para ver si se copia tambien el tipo
	
	void Reset() { type.reset(); value = std::monostate(); }
	
	static DataValue MakeEmpty(tipo_var t) { return DataValue(t); }
	static DataValue MakeInt(int i) { return DataValue(vt_numerica_entera,i); }
	static DataValue MakeReal(double d) { return DataValue(vt_numerica,d); }
	static DataValue MakeLogic(bool b) { return DataValue(vt_logica,b); }
	static DataValue MakeString(const std::string &s) { return DataValue(vt_caracter,s); }
	
	static DataValue MakeReal(const std::string &s) { return DataValue(vt_numerica,StrToDbl(s)); }
	static DataValue MakeLogic(const std::string &s) { return DataValue(vt_logica,s==VERDADERO); }
	
	static DataValue MakeError() { return DataValue::MakeEmpty(vt_error); }
};

#ifdef __APPLE__
namespace std {
	template<typename T>
	auto &get(const DataValue::variant_t &inst) { return *std::get_if<T>(&inst); }
}
#endif

inline bool DataValue::GetAsBool() const {
	if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
	else if (std::holds_alternative<std::string>(value)) {
		const std::string &str = std::get<std::string>(value);
		return !str.empty() && (str[0]=='1'||toupper(str[0]=='V'));
	}
	else if (std::holds_alternative<double>(value)) return std::get<double>(value)==1;
	else if (std::holds_alternative<int>(value)) return std::get<int>(value)==1;
	else return false;
}

inline double DataValue::GetAsReal() const {
	_expects(!IsEmpty()||!std::holds_alternative<bool>(value));
	if (std::holds_alternative<double>(value))      return std::get<double>(value);
	if (std::holds_alternative<int>(value))         return double(std::get<int>(value));
	if (std::holds_alternative<std::string>(value)) return StrToDbl(std::get<std::string>(value));
	return 0.0;
}

inline int DataValue::GetAsInt() const {
	_expects(!IsEmpty()||!std::holds_alternative<bool>(value));
	if (std::holds_alternative<double>(value))      return int(std::get<double>(value));
	if (std::holds_alternative<int>(value))         return std::get<int>(value);
	if (std::holds_alternative<std::string>(value)) return StrToInt(std::get<std::string>(value));
	return 0;
}

inline std::string DataValue::GetAsString() const {
	_expects(IsEmpty()||std::holds_alternative<std::string>(value));
	if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
	return "";
}

inline std::string DataValue::GetForUser() const {
	if (type==vt_numerica) return DblToStr(GetAsReal(),true); // si era numerica, redondear para salida
	if (type==vt_logica) return GetAsBool()?VERDADERO:FALSO;
	return GetAsString();
}

#endif
