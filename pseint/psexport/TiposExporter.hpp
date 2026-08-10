#ifndef TIPOS_EXPORTER_HPP
#define TIPOS_EXPORTER_HPP

#include "ExporterBase.hpp"

class TiposExporter : public ExporterBase {
	bool switch_only_for_integers;
protected:
	void translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) override;
	void esperar_tiempo(t_output &prog, std::string tiempo, bool mili, std::string tabs) override;
	void esperar_tecla(t_output &prog, std::string tabs) override;
	void dimension(t_output &prog, t_arglist &nombre, t_arglist &tamanios, std::string tabs) override;
	void borrar_pantalla(t_output &prog, std::string tabs) override;
	void invocar(t_output &prog, std::string func_name, std::string args, std::string tabs) override;
	void escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs) override;
	void leer(t_output &prog, t_arglist args, std::string tabs) override;
	void asignacion(t_output &prog, std::string param1, std::string param2, std::string tabs) override;
	void si(t_output &prog, t_proceso_it r, t_proceso_it q, t_proceso_it s, std::string tabs) override;
	void mientras(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) override;
	void repetir(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void para(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void paracada(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void definir(t_output &prog, t_arglist &arglist, tipo_var tipo, std::string tabs) override;
	
public:
	std::string make_string(std::string cont);
	std::string function(std::string name, std::string args);
	std::string get_constante(std::string name);
	std::string get_operator(std::string op, bool for_string=false);	
	void translate(t_output &out, Programa &prog) override;
	TiposExporter(Programa &prog, bool switch_only_for_integers);
	
};

#endif

