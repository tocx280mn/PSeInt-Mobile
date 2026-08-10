#ifndef INSTRUCCION_HPP
#define INSTRUCCION_HPP

#include <string>
#include <vector>
#include <variant>
#include "debug.h"
#include "DataValue.h"
#include "Code.h"

enum InstructionType {
	IT_NULL, IT_ERROR, IT_COMMENT, IT_PROCESO, IT_FINPROCESO,
	IT_LEER, IT_ASIGNAR, IT_ESCRIBIR, IT_DIMENSION, IT_DEFINIR,
	IT_ESPERARTECLA, IT_ESPERAR, IT_BORRARPANTALLA, IT_INVOCAR,
	IT_MIENTRAS, IT_FINMIENTRAS, IT_REPETIR, IT_HASTAQUE,
	IT_SI, IT_ENTONCES, IT_SINO, IT_FINSI, IT_SEGUN, IT_OPCION, IT_DEOTROMODO, IT_FINSEGUN,
	IT_PARA, IT_PARACADA, IT_FINPARA
};

struct Instruccion {
	InstructionType type;
	std::string instruccion;
	CodeLocation loc;
	Instruccion(std::string _instruccion, CodeLocation _loc)
		:type(IT_NULL),instruccion(_instruccion),loc(_loc){}
//	Instruccion(InstructionType _type, int _num_linea=-1, int _num_instruccion=-1)
//		:type(_type),instruccion(""),num_linea(_num_linea),num_instruccion(_num_instruccion){}
//	operator std::string() { return instruccion; }
	bool operator==(InstructionType t) const { return type==t; }
	bool operator!=(InstructionType t) const { return type!=t; }
	std::string &operator=(const std::string &s) { return instruccion=s; }
	
	struct INull {};
	
	struct IProceso {
		std::string nombre, ret_id, args;
		bool principal;
		int fin = -1;
	};
	
	struct IFinProceso {
		std::string nombre;
		bool principal;
	};
	
	struct IEscribir {
		std::vector<std::string> expresiones;
		bool saltar = true;
	};
	
	struct IAsignar {
		std::string variable, valor;
	};
	
	struct ILeer {
		std::vector<std::string> variables;
	};
	
	struct ISi {
		std::string condicion;
		int sino = -1, fin = -1;
	};

	struct IMientras {
		std::string condicion;
		int fin = -1;
	};
	
	struct IHastaQue {
		std::string condicion;
		bool mientras_que = false;
	};
	
	struct IDimension {
		std::vector<std::string> nombres;
		std::vector<std::string> tamanios;
		bool redimension = false;
	};
	
	struct IPara {
		std::string contador, val_ini, paso, val_fin;
		int fin = -1;
	};
	
	struct IParaCada {
		std::string identificador, arreglo;
		int fin = -1;
	};
	
	struct IRepetir {
		int fin = -1;
	};
	
	struct ISegun {
		std::string expresion;
		std::vector<int> opciones;
		int fin = -1;
	};
	
	struct IOpcion {
		std::vector<std::string> expresiones;
		int siguiente;
	};
	
	struct IEsperar {
		std::string tiempo;
		int factor = 1;
	};
	
	struct IDefinir {
		std::vector<std::string> variables;
		tipo_var tipo;
	};
	
	struct IInvocar {
		std::string nombre, args;
	};
	
	struct IComentario {
		std::string text;
		// bool is_inline; // no es necesario, se puede preguntar si inst.loc.instruccion!=1
	};
	
	using variant_t = std::variant<INull,IComentario,IProceso,IFinProceso,IEscribir,ILeer,IAsignar,
		                           IDimension,IDefinir,IEsperar,IInvocar,
				                   ISi,ISegun,IOpcion,IHastaQue,IPara,IParaCada,IMientras,IRepetir>;
	variant_t impl;
	void setType(InstructionType t) {
		type = t;
		switch(type) {
		case IT_ESCRIBIR:   impl = IEscribir();   break;
		case IT_LEER:       impl = ILeer();       break;
		case IT_ASIGNAR:    impl = IAsignar();    break;
		case IT_SI:         impl = ISi();         break;
		case IT_HASTAQUE:   impl = IHastaQue();   break;
		case IT_DEFINIR:    impl = IDefinir();    break;
		case IT_DIMENSION:  impl = IDimension();  break;
		case IT_PARA:       impl = IPara();       break;
		case IT_PARACADA:   impl = IParaCada();   break;
		case IT_MIENTRAS:   impl = IMientras();   break;
		case IT_SEGUN:      impl = ISegun();      break;
		case IT_OPCION:     impl = IOpcion();     break;
		case IT_ESPERAR:    impl = IEsperar();    break;
		case IT_PROCESO:    impl = IProceso();    break;
		case IT_FINPROCESO: impl = IFinProceso(); break;
		case IT_INVOCAR:    impl = IInvocar();    break;
		case IT_REPETIR:    impl = IRepetir();    break;
		case IT_COMMENT:    impl = IComentario(); break;
		default: ; // las demas instrucciones por ahora no tienen info adicional
		}
	}
};

namespace std {
	template<typename T>
	auto &get(Instruccion::variant_t &inst) { return *std::get_if<T>(&inst); }
}


// estos strucs auxiliares son para que podamos pedir el struct con los detalles
// de cada tipo de instruccion sin tener que saber su tipo (el del struct), ni
// como se llama el atributo, o si es herencia, o lo que sea...
// simplemente: auto &impl = getImpl<IT_LEER>(inst);
// siempre que algo varía con el tipo de instrucción, mejor que el código cliente
// dependa solo del enum InstruccionType y el resto de los detalles queden escondidos
template<int IT_ALGO> struct InstImplHelper { };
template<> struct InstImplHelper<IT_ASIGNAR>   { static auto& get(Instruccion &inst) { return std::get<Instruccion::IAsignar>   (inst.impl); } };
template<> struct InstImplHelper<IT_ESCRIBIR>  { static auto& get(Instruccion &inst) { return std::get<Instruccion::IEscribir>  (inst.impl); } };
template<> struct InstImplHelper<IT_LEER>      { static auto& get(Instruccion &inst) { return std::get<Instruccion::ILeer>      (inst.impl); } };
template<> struct InstImplHelper<IT_SI>        { static auto& get(Instruccion &inst) { return std::get<Instruccion::ISi>        (inst.impl); } };
template<> struct InstImplHelper<IT_HASTAQUE>  { static auto& get(Instruccion &inst) { return std::get<Instruccion::IHastaQue>  (inst.impl); } };
template<> struct InstImplHelper<IT_DEFINIR>   { static auto& get(Instruccion &inst) { return std::get<Instruccion::IDefinir>   (inst.impl); } };
template<> struct InstImplHelper<IT_DIMENSION> { static auto& get(Instruccion &inst) { return std::get<Instruccion::IDimension> (inst.impl); } };
template<> struct InstImplHelper<IT_MIENTRAS>  { static auto& get(Instruccion &inst) { return std::get<Instruccion::IMientras>  (inst.impl); } };
template<> struct InstImplHelper<IT_PARA>      { static auto& get(Instruccion &inst) { return std::get<Instruccion::IPara>      (inst.impl); } };
template<> struct InstImplHelper<IT_PARACADA>  { static auto& get(Instruccion &inst) { return std::get<Instruccion::IParaCada>  (inst.impl); } };
template<> struct InstImplHelper<IT_OPCION>    { static auto& get(Instruccion &inst) { return std::get<Instruccion::IOpcion>    (inst.impl); } };
template<> struct InstImplHelper<IT_SEGUN>     { static auto& get(Instruccion &inst) { return std::get<Instruccion::ISegun>     (inst.impl); } };
template<> struct InstImplHelper<IT_ESPERAR>   { static auto& get(Instruccion &inst) { return std::get<Instruccion::IEsperar>   (inst.impl); } };
template<> struct InstImplHelper<IT_PROCESO>   { static auto& get(Instruccion &inst) { return std::get<Instruccion::IProceso>   (inst.impl); } };
template<> struct InstImplHelper<IT_FINPROCESO>{ static auto& get(Instruccion &inst) { return std::get<Instruccion::IFinProceso>(inst.impl); } };
template<> struct InstImplHelper<IT_INVOCAR>   { static auto& get(Instruccion &inst) { return std::get<Instruccion::IInvocar>   (inst.impl); } };
template<> struct InstImplHelper<IT_REPETIR>   { static auto& get(Instruccion &inst) { return std::get<Instruccion::IRepetir>   (inst.impl); } };
template<> struct InstImplHelper<IT_COMMENT>   { static auto& get(Instruccion &inst) { return std::get<Instruccion::IComentario>(inst.impl); } };
template<int IT_ALGO> auto &getImpl(Instruccion &inst) { _expects(inst.type==IT_ALGO); return InstImplHelper<IT_ALGO>::get(inst); }

#endif
