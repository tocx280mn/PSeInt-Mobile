#ifndef EXPORT_CPP_H
#define EXPORT_CPP_H

#include "ExporterBase.hpp"

class CppExporter : public ExporterBase {
	
protected:
	
	bool include_cmath = false;
	bool include_cstdlib = false;
	bool use_sin_tipo = false;
	bool use_string = false;
	bool use_func_esperar = false;
	bool use_func_minusculas = false;
	bool use_func_mayusculas = false;
	bool use_func_convertiratexto = false;
	bool use_arreglo_max = false;
	bool has_matrix_func = false;
	bool read_strings = true;
	bool prints_bool = false;
	t_output prototipos; // forward declarations de las funciones
	
	virtual std::string convertirAString(const std::string &s);
	
	virtual std::string get_tipo(std::map<std::string,tipo_var>::iterator &mit, bool for_func=false, bool by_ref=false); // se usa tanto desde el otro get_tipo como desde declarar_variables
	virtual void declarar_variables(t_output &prog, std::string tab="\t", bool ignore_arrays=false);
	virtual std::string get_tipo(std::string name, bool by_ref=false, bool do_erase=true); // solo se usa para cabeceras de funciones
	virtual void header(t_output &out);
	virtual void footer(t_output &out);
	virtual void translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) override;
	virtual void translate_all_procs(t_output &out, Programa &prog, std::string tabs="") override;
	
	virtual void esperar_tiempo(t_output &prog, std::string tiempo, bool mili, std::string tabs) override;
	virtual void esperar_tecla(t_output &prog, std::string tabs) override;
	virtual void borrar_pantalla(t_output &prog, std::string tabs) override;
	virtual void invocar(t_output &prog, std::string func_name, std::string params, std::string tabs) override;
	virtual void escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs) override;
	virtual void leer(t_output &prog, t_arglist args, std::string tabs) override;
	virtual void asignacion(t_output &prog, std::string param1, std::string param2, std::string tabs) override;
	virtual void si(t_output &prog, t_proceso_it it_si, t_proceso_it it_sino, t_proceso_it it_fin, std::string tabs) override;
	virtual void mientras(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	virtual void segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) override;
	virtual void repetir(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	virtual void para(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	virtual void paracada(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	virtual void comentar(t_output &prog, std::string text, std::string tabs) override;
	
public:
	virtual std::string make_string(std::string cont);
	virtual std::string function(std::string name, std::string args);
	virtual std::string get_constante(std::string name);
	virtual std::string get_operator(std::string op, bool for_string=false);	
	virtual void translate(t_output &out, Programa &prog);
	CppExporter();
	
};

#endif

