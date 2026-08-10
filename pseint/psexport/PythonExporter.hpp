#ifndef PYTHON_EXPORTER_HPP
#define PYTHON_EXPORTER_HPP

#include "ExporterBase.hpp"

class PythonExporter : public ExporterBase {
	
	int version;
	
	bool import_pi;
	bool import_sleep;
	bool import_sqrt;
	bool import_log;
	bool import_exp;
	bool import_sin;
	bool import_cos;
	bool import_tan;
	bool import_asin;
	bool import_acos;
	bool import_atan;
	bool import_randint;
	bool use_subprocesos;
	
protected:
	
	void header(t_output &out);
	void translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) override;
	
	void definir(t_output &prog, t_arglist &variables, tipo_var tipo, std::string tabs) override;
	void dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs) override;
	
	void esperar_tiempo(t_output &prog, std::string tiempo, bool mili, std::string tabs) override;
	void esperar_tecla(t_output &prog, std::string tabs) override;
	void borrar_pantalla(t_output &prog, std::string tabs) override;
	void invocar(t_output &prog, std::string func_name, std::string args, std::string tabs) override;
	void escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs) override;
	void leer(t_output &prog, t_arglist args, std::string tabs) override;
	void asignacion(t_output &prog, std::string param1, std::string param2, std::string tabs) override;
	void si(t_output &prog, t_proceso_it it_si, t_proceso_it it_sino, t_proceso_it it_fin, std::string tabs) override;
	void mientras(t_output &prog, t_proceso_it it_mientras, t_proceso_it it_fin, std::string tabs) override;
	void segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) override;
	void repetir(t_output &prog, t_proceso_it it_repetir, t_proceso_it it_hasta, std::string tabs) override;
	void para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) override;
	void paracada(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) override;
	void comentar(t_output &prog, std::string text, std::string tabs) override;
	
public:
	std::string make_string (std::string cont) override;
	std::string function(std::string name, std::string args) override;
	std::string get_constante(std::string name) override;
	std::string get_operator(std::string op, bool for_string=false) override;
	void translate(t_output &out, Programa &prog) override;
	PythonExporter(int version);
	
};

#endif

