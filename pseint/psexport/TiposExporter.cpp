#include <sstream>
#include <cstdlib>
#include "TiposExporter.hpp"
#include "version.h"
#include "exportexp.h"
#include "ExporterBase.hpp"
using namespace std;

static void AplicarTipo(RunTime &rt, const std::string &s, tipo_var t) {
	int i=0, j=s.size()-1;
	AplicarTipo(rt,s,i,j,t);
}

TiposExporter::TiposExporter(Programa &prog, bool switch_only_for_integers) {
	this->switch_only_for_integers=switch_only_for_integers;
	t_output out; translate(out,prog);
}

void TiposExporter::esperar_tiempo(t_output &prog, std::string tiempo, bool mili, std::string tabs) {
	AplicarTipo(GetRT(),tiempo,vt_numerica);
}

void TiposExporter::dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs) {
	for(size_t i=0;i<nombres.size();++i) { 
		std::string args = tamanios[i];
		t_arglist dims = splitArgsList(args);
		for (auto &d : dims)
			AplicarTipo(GetRT(),d,vt_numerica_entera);
	}
}

void TiposExporter::esperar_tecla(t_output &prog, std::string tabs) { }

void TiposExporter::borrar_pantalla(t_output &prog, std::string tabs) { }

void TiposExporter::invocar(t_output &prog, std::string func_name, std::string params, std::string tabs) { }

void TiposExporter::escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs){ }

void TiposExporter::leer(t_output &prog, t_arglist args, std::string tabs) { }

void TiposExporter::asignacion(t_output &prog, std::string variable, std::string valor, std::string tabs) {
	tipo_var t;
	expresion(valor,t);
	AplicarTipo(GetRT(),variable,t);
}

void TiposExporter::si(t_output &prog, t_proceso_it it_si, t_proceso_it it_sino, t_proceso_it it_fin, std::string tabs){
	AplicarTipo(GetRT(),getImpl<IT_SI>(*it_si).condicion,vt_logica);
	bloque(prog,std::next(it_si),it_sino,tabs+"\t");
	if (it_sino!=it_fin) bloque(prog,++it_sino,it_fin,tabs+"\t");
}

void TiposExporter::mientras(t_output &prog, t_proceso_it it_mientras, t_proceso_it it_fin, std::string tabs){
	AplicarTipo(GetRT(),getImpl<IT_MIENTRAS>(*it_mientras).condicion,vt_logica);
	bloque(prog,std::next(it_mientras),it_fin,tabs+"\t");
}

void TiposExporter::segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) {
	std::string cond = getImpl<IT_SEGUN>(*(its[0])).expresion;
	
	for(size_t i=1;i+1<its.size();++i)
		bloque(prog,std::next(its[i]),its[i+1],tabs+"\t");
	
	tipo_var t;
	if (switch_only_for_integers) {
		t = vt_numerica_entera;
	} else {
		expresion(cond,t);
		for(auto it : its) {
			if (it->type!=IT_OPCION) continue;
			for(std::string &exp : getImpl<IT_OPCION>(*it).expresiones) {
				tipo_var aux;
				expresion(exp,aux);
				if (!t.set(aux)) return;
			}
		}
	}
	
	AplicarTipo(GetRT(),cond,t);
	for(auto it : its) {
		if (it->type!=IT_OPCION) continue;
		for(std::string &exp : getImpl<IT_OPCION>(*it).expresiones)
			AplicarTipo(GetRT(),exp,t);
	}
	
}

void TiposExporter::repetir(t_output &prog, t_proceso_it it_repetir, t_proceso_it it_hasta, std::string tabs){
	AplicarTipo(GetRT(),getImpl<IT_HASTAQUE>(*it_hasta).condicion,vt_logica);
	bloque(prog,std::next(it_repetir),it_hasta,tabs+"\t");
}

void TiposExporter::para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs){
	auto &impl = getImpl<IT_PARA>(*it_para);
	std::string var = expresion(impl.contador), ini = expresion(impl.val_ini),
		        fin = expresion(impl.val_fin), paso = impl.paso;
	AplicarTipo(GetRT(),var,vt_numerica);
	AplicarTipo(GetRT(),ini,vt_numerica);
	AplicarTipo(GetRT(),fin,vt_numerica);
	AplicarTipo(GetRT(),paso,vt_numerica);
	bloque(prog,std::next(it_para),it_fin,tabs+"\t");
}

void TiposExporter::paracada(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) {
	auto &impl = getImpl<IT_PARACADA>(*it_para);
	std::string identif = ToLower(impl.identificador);
	std::string arreglo = ToLower(impl.arreglo);
	memoria->DefinirTipo(identif,memoria->LeerTipo(arreglo));
	bloque(prog,std::next(it_para),it_fin,tabs+"\t");
	memoria->DefinirTipo(arreglo,memoria->LeerTipo(identif));
	memoria->RemoveVar(identif);
}

string TiposExporter::function(std::string name, std::string args) {
	return ToLower(name)+args; 
}

void TiposExporter::translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) {
	t_output out_proc; 
	bloque(out_proc,++proc.begin(),proc.end(),"\t");
	memoria=NULL; // para que translate_all_procs no la elimine
}

void TiposExporter::translate(t_output &out, Programa &prog) {
	ExporterBase *old=exporter;
	exporter=this;
	translate_all_procs(out,prog);
	exporter=old;
}

string TiposExporter::get_constante(std::string name) { return name; }

string TiposExporter::get_operator(std::string op, bool for_string) { return op; }

string TiposExporter::make_string(std::string cont) { return string("\'")+cont+"\'"; }

void TiposExporter::definir(t_output &prog, t_arglist &variables, tipo_var tipo, std::string tabs) {
	for(std::string &var : variables)
		memoria->DefinirTipo(expresion(var),tipo,tipo.rounded);
}
