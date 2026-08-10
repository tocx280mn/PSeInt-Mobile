#ifndef EXPORT_C_H
#define EXPORT_C_H

#include "ExporterBase.hpp"
#include "CppExporter.hpp"

class CExporter : public CppExporter {
	
	bool declare_cstrings;
	bool use_bool;
	bool use_get_aux_buffer;
	bool use_func_subcadena;
	bool use_reference;
	
	std::string get_tipo(std::map<std::string,tipo_var>::iterator &mit, bool for_func=false, bool by_ref=false); // se usa tanto desde el otro get_tipo como desde declarar_variables
//	void declarar_variables(t_output &prog);
//	string get_tipo(string name, bool by_ref=false); // solo se usa para cabeceras de funciones
	void header(t_output &out);
	void footer(t_output &out);
	void translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) override;
	
//	void esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) override;
	void esperar_tecla(t_output &prog, std::string tabs) override;
	void borrar_pantalla(t_output &prog, std::string tabs) override;
//	void invocar(t_output &prog, std::string func_name, std::string params, std::string tabs) override;
	void escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs) override;
	void leer(t_output &prog, t_arglist args, std::string tabs) override;
//	void asignacion(t_output &prog, string param1, string param2, std::string tabs) override;
//	void si(t_output &prog, t_proceso_it r, t_proceso_it q, t_proceso_it s, std::string tabs) override;
//	void mientras(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
//	void segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) override;
//	void repetir(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void para(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void paracada(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void comentar(t_output &prog, std::string text, std::string tabs) override;
	
public:
	std::string referencia(const std::string &exp) override;
	std::string function(std::string name, std::string args) override;
//	string get_constante(string name) override;
	std::string get_operator(std::string op, bool for_string=false) override;	
//	void translate(t_output &out, t_programa &prog) override;
	CExporter();
	
};

#endif

