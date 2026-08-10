#include <stack>
#include "PythonExporter.hpp"
#include "version.h"
#include "exportexp.h"
#include "ExporterBase.hpp"
#include "TiposExporter.hpp"
#include <algorithm>
using namespace std;

PythonExporter::PythonExporter(int version) {
	this->version=version;
	import_pi=false;
	import_sleep=false;
	import_sqrt=false;
	import_log=false;
	import_exp=false;
	import_sin=false;
	import_cos=false;
	import_tan=false;
	import_asin=false;
	import_acos=false;
	import_atan=false;
	import_randint=false;
	use_subprocesos=false;
	output_base_zero_arrays=true;
}

void PythonExporter::dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs) {
	ExporterBase::dimension(prog,nombres,tamanios,tabs); // creo que esto no es necesario, pero lo dejo por las dudas (del copy/paste de java)
	for(size_t i=0;i<nombres.size();++i) {
		// obtener nombre y dimensiones
		string name = nombres[i], dims = tamanios[i];
		t_arglist dimlist = splitArgsList(dims);
		// obtener un elemento del tipo adecuado (para replicar por todo el arreglo)	
		string stipo = "str()";
		tipo_var tipo = memoria->LeerTipo(name);
		if (tipo==vt_numerica) stipo = tipo.rounded?"int()":"float()";
		if (tipo==vt_logica) stipo = "bool()";
		// armar la expresion que genera el arreglo
		dims = stipo;
		std::reverse(dimlist.begin(),dimlist.end());
		t_arglist_it it2 = dimlist.begin();
		stack<string> auxvars;
		while (it2!=dimlist.end()) {
			auxvars.push(get_aux_varname("ind"));
			dims=string("[")+dims+" for "+auxvars.top()+" in range("+expresion(*it2)+")]";
			++it2;
		}
		while(!auxvars.empty()) { release_aux_varname(auxvars.top()); auxvars.pop(); }
		// asignar
		insertar(prog,tabs+ToLower(name)+" = "+dims);
	}
}

void PythonExporter::borrar_pantalla(t_output &out, std::string tabs) {
	string linea=tabs;
	if (version==3) linea+="print(\"\")";
	else linea+="print \"\"";
	if (!for_test) linea+=" # no hay forma directa de borrar la pantalla con Python estandar";
	insertar(out,linea);
}

void PythonExporter::esperar_tecla(t_output &prog, std::string tabs){
	string input=version==3?"input":"raw_input";
	if (for_test)
		insertar(prog,tabs+input+"()");
	else
		insertar(prog,tabs+input+"() # a diferencia del pseudocódigo, espera un Enter, no cualquier tecla");
}

void PythonExporter::esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) {
	tipo_var t; tiempo = expresion(tiempo,t); // para que arregle los nombres de las variables
	import_sleep=true;
	if (mili) 
		insertar(prog,tabs+"sleep("+colocarParentesis(tiempo)+"/1000.0)");
	else
		insertar(prog,tabs+"sleep("+tiempo+")");
}

void PythonExporter::invocar(t_output &prog, std::string func_name, std::string args, std::string tabs){
	if (args.empty()) args = "()";
	string linea = expresion(func_name+args);
	insertar(prog, tabs+linea);
}

void PythonExporter::escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs){
	t_arglist_it it=args.begin();
	string linea="print",sep=version==3?"(":" ";
	while (it!=args.end()) {
		linea+=sep; sep=",";
		linea+=expresion(*it);
		++it;
	}
	if (version==3) {
		linea+=(saltar?")":", end=\"\")");
	} else if (!saltar) linea+=",";
	insertar(prog,tabs+linea);
}

void PythonExporter::definir(t_output &prog, t_arglist &variables, tipo_var tipo, std::string tabs) {
	std::string str_tipo;
	if (tipo==vt_numerica) {
		if (tipo.rounded) str_tipo = "int()";
		else str_tipo = "float()";
	}
	else if (tipo==vt_logica) str_tipo = "bool()";
	else str_tipo = "str()";
	for(auto &var : variables)
		insertar(prog, tabs+expresion(var)+" = "+str_tipo);
}


void PythonExporter::leer(t_output &prog, t_arglist args, std::string tabs) {
	string input=version==3?"input":"raw_input";
	t_arglist_it it=args.begin();
	while (it!=args.end()) {
		tipo_var t;
		string varname=expresion(*it,t);
		if (t==vt_numerica && t.rounded) insertar(prog,tabs+varname+" = int("+input+"())");
		else if (t==vt_numerica) insertar(prog,tabs+varname+" = float("+input+"())");
		else if (t==vt_logica) insertar(prog,tabs+varname+" = bool("+input+"())");
		else  insertar(prog,tabs+varname+" = "+input+"()");
		++it;
	}
}

void PythonExporter::asignacion(t_output &prog, string param1, string param2, std::string tabs){
	insertar(prog,tabs+expresion(param1)+" = "+expresion(param2));
}

void PythonExporter::si(t_output &prog, t_proceso_it it_si, t_proceso_it it_sino, t_proceso_it it_fin, std::string tabs){
	insertar(prog,tabs+"if "+expresion(getImpl<IT_SI>(*it_si).condicion)+":");
	bloque(prog,std::next(it_si),it_sino,tabs+"\t");
	if (it_sino!=it_fin) {
		insertar(prog,tabs+"else:");
		bloque(prog,std::next(it_sino),it_fin,tabs+"\t");
	}
}

void PythonExporter::mientras(t_output &prog, t_proceso_it it_mientras, t_proceso_it it_fin, std::string tabs){
	insertar(prog,tabs+"while "+expresion(getImpl<IT_MIENTRAS>(*it_mientras).condicion)+":");
	bloque(prog,std::next(it_mientras),it_fin,tabs+"\t");
}

void PythonExporter::segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) {
	string cond = expresion(getImpl<IT_SEGUN>(*(its[0])).expresion)+"==";
	bool first=true;
	for(size_t i=1;i+1<its.size();++i) {
		if (its[i]->type==IT_DEOTROMODO) {
			insertar(prog,tabs+"else:");
		} else {
			string line = first ? "if " : "elif "; first = false;
			auto &vexprs = getImpl<IT_OPCION>(*(its[i])).expresiones;
			for(size_t j=0;j<vexprs.size();++j) {
				if (j!=0) line += " or ";
				line += cond + expresion(vexprs[j]);
			}
			line += ":";
			insertar(prog, tabs+line);
		}
		bloque(prog, std::next(its[i]),its[i+1], tabs+"\t");
	}
}

void PythonExporter::repetir(t_output &prog, t_proceso_it it_repetir, t_proceso_it it_hasta, std::string tabs){
	insertar(prog, tabs+"while True:"+(for_test?"":"# no hay 'repetir' en python"));
	bloque(prog, std::next(it_repetir), it_hasta, tabs+"\t");
	auto &impl = getImpl<IT_HASTAQUE>(*it_hasta);
	if (impl.mientras_que)
		insertar(prog, tabs+"\tif "+expresion(invert_expresion(impl.condicion))+": break");
	else
		insertar(prog, tabs+"\tif "+expresion(impl.condicion)+": break");
}

void PythonExporter::para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs){
	auto &impl = getImpl<IT_PARA>(*it_para);
	std::string var = expresion(impl.contador), ini = expresion(impl.val_ini), 
		        fin = expresion(impl.val_fin), paso = impl.paso;
	std::string line = string("for ")+var+" in range(";
	if (ini!="0"||paso!="1") line += ini+",";
	line += sumarOrestarUno(fin, paso.empty() or paso[0]!='-');
	if (paso!="1" and (not paso.empty())) line += string(",")+paso;
	line += "):";
	insertar(prog, tabs+line);
	bloque(prog, std::next(it_para), it_fin, tabs+"\t");
}

void PythonExporter::paracada(t_output &out, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) {
	auto &impl = getImpl<IT_PARACADA>(*it_para);
	// el "for x in a" de python sirve para solo-lectura (modificar x no modifica a)
	string arreglo = ToLower(impl.arreglo), identif = ToLower(impl.identificador);
	const int *dims = memoria->LeerDims(arreglo);
	int n = dims[0];
	
	string *auxvars=new string[n];
	for(int i=0;i<n;i++) auxvars[i]=get_aux_varname("aux_index_");
	
	string vname=arreglo;
	for(int i=0;i<n;i++) { 
		string idx=auxvars[i];
		insertar(out,tabs+"for "+idx+" in range("+IntToStr(dims[i+1])+"):");
		vname+="["+idx+"]";
		tabs+="\t";
	}
	
	for(int i=n-1;i>=0;i--) release_aux_varname(auxvars[i]);
	delete []auxvars;
	
	t_output aux_out;
	bloque(aux_out,std::next(it_para),it_fin,tabs);
	replace_var(aux_out,identif,vname);
	insertar_out(out,aux_out);
	memoria->RemoveVar(identif);
}

string PythonExporter::function(string name, string args) {
	if (name=="SEN") {
		import_sin=true;
		return string("sin")+args;
	} else if (name=="TAN") {
		import_tan=true;
		return string("tan")+args;
	} else if (name=="ASEN") {
		import_asin=true;
		return string("asin")+args;
	} else if (name=="ACOS") {
		import_acos=true;
		return string("acos")+args;
	} else if (name=="COS") {
		import_cos=true;
		return string("cos")+args;
	} else if (name=="RAIZ") {
		import_sqrt=true;
		return string("sqrt")+args;
	} else if (name=="RC") {
		import_sqrt=true;
		return string("sqrt")+args;
	} else if (name=="ABS") {
		return string("abs")+args;
	} else if (name=="LN") {
		import_log=true;
		return string("log")+args;
	} else if (name=="EXP") {
		import_exp=true;
		return string("exp")+args;
	} else if (name=="AZAR") {
		import_randint=true;
		return string("randint(0,")+sumarOrestarUno(get_arg(args,1),false)+")";
	} else if (name=="ATAN") {
		import_atan=true;
		return string("atan")+args;
	} else if (name=="TRUNC") {
		return string("int")+args;
	} else if (name=="REDON") {
		return string("round")+args;
	} else if (name=="CONCATENAR") {
		return get_arg(args,1)+"+"+get_arg(args,2);
	} else if (name=="LONGITUD") {
		return string("len")+args;
	} else if (name=="SUBCADENA") {
		string desde=get_arg(args,2);
		if (!input_base_zero_arrays) desde=sumarOrestarUno(desde,false);
		string hasta=get_arg(args,3);
		if (input_base_zero_arrays) hasta=sumarOrestarUno(hasta,true);
		return get_arg(args,1)+"["+desde+":"+hasta+"]";
	} else if (name=="CONVERTIRANUMERO") {
		return string("float")+args;
	} else if (name=="MAYUSCULAS") {
		return string("str.upper")+args;
	} else if (name=="MINUSCULAS") {
		return string("str.lower")+args;
	} else if (name=="CONVERTIRATEXTO") {
		return string("str")+args;
	} else {
		return name+args; // no debería pasar nunca
	}
}

void PythonExporter::header(t_output &out) {
	// cabecera
	if (version==2) insertar(out,"# -*- coding: iso-8859-15 -*-");
	init_header(out,"# ");
	string math_stuff;
	if (import_pi) math_stuff+=", pi";
	if (import_sqrt) math_stuff+=", sqrt";
	if (import_log) math_stuff+=", log";
	if (import_exp) math_stuff+=", exp";
	if (import_sin) math_stuff+=", sin";
	if (import_cos) math_stuff+=", cos";
	if (import_tan) math_stuff+=", tan";
	if (import_asin) math_stuff+=", asin";
	if (import_acos) math_stuff+=", acos";
	if (import_atan) math_stuff+=", atan";
	if (math_stuff.size()) out.push_back(string("from math import")+math_stuff.substr(1));
	if (import_randint) out.push_back("from random import randint");
	if (import_sleep) out.push_back("from time import sleep");
	if (!for_test) out.push_back("");
	if (!for_test && use_subprocesos) {
		out.push_back("# En python no hay forma de elegir como pasar una variable a una");
		out.push_back("# funcion (por referencia o por valor). Las variables \"inmutables\"");
		out.push_back("# (str, int, float, bool) se pasan siempre por copia mientras que");
		out.push_back("# las demas (listas, objetos, etc.) se pasan siempre por referencia.");
		out.push_back("# Esto coincide con el comportamiento por defecto en pseint, pero");
		out.push_back("# cuando utiliza las palabras clave \"Por Referencia\" para");
		out.push_back("# alterarlo la traducción no es correcta.");
		out.push_back("");
	}
}

void PythonExporter::translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) {
	
	if (f) use_subprocesos=true;
	
	//cuerpo del proceso
	t_output out_proc;
	bloque(out_proc,++proc.begin(),proc.end(),"\t");
	
	// cabecera del proceso
	string ret; // sentencia "return ..." de la funcion
	if (!f) {
		out.push_back("if __name__ == '__main__':");
	} else {
		string dec="def ";
		dec+=ToLower(f->id)+"(";
		for(int i=1;i<=f->GetArgsCount();i++) {
			if (i!=1) dec+=", ";
			dec+=ToLower(f->nombres[i]);
		}
		dec+="):";
		if (f->nombres[0]!="") {
			ret=string("return ")+ToLower(f->nombres[0]);
		}
		out.push_back(dec);
	}
	
	//cuerpo del proceso
	insertar_out(out,out_proc);
	
	// cola del proceso
	if (ret.size()) out.push_back(string("\t")+ret);
	if (!for_test) out.push_back("");
}

void PythonExporter::translate(t_output &out, Programa &prog) {
	load_subs_in_funcs_manager(prog);
	// cppcheck-suppress unusedScopedObject
	TiposExporter(prog,false); // para que se cargue el mapa_memorias con memorias que tengan ya definidos los tipos de variables que correspondan
	
	t_output aux_defs,aux_main;
	translate_all_procs(aux_main,aux_defs,prog);
	
	header(out);
	insertar_out(out,aux_defs);
	insertar_out(out,aux_main);
}

string PythonExporter::get_constante(string name) {
	if (name=="PI") { import_pi=true; return "pi"; }
	if (name=="VERDADERO") return "True";
	if (name=="FALSO") return "False";
	return name;
}

string PythonExporter::get_operator(string op, bool for_string) {
	// para agrupar operaciones y alterar la jerarquia
	if (op=="(") return "("; 
	if (op==")") return ")";
	// para llamadas a funciones
	if (op=="{") return "("; 
	if (op=="}") return ")";
	// para indices de arreglos
	if (op=="[") return "[";
	if (op==",") return "][";
	if (op=="]") return "]";
	// algebraicos, logicos, relaciones
	if (op=="+") return "+"; 
	if (op=="-") return "-"; 
	if (op=="/") return "/"; 
	if (op=="*") return "*";
	if (op=="^") return "**";
	if (op=="%") return "%";
	if (op=="=") return "=="; 
	if (op=="<>") return "!="; 
	if (op=="<") return "<"; 
	if (op==">") return ">"; 
	if (op=="<=") return "<="; 
	if (op==">=") return ">="; 
	if (op=="&") return " and "; 
	if (op=="|") return " or "; 
	if (op=="~") return "not "; 
	return op; // no deberia pasar nunca
}

string PythonExporter::make_string (string cont) {
	for(unsigned int i=0;i<cont.size();i++)
		if (cont[i]=='\\') cont.insert(i++,"\\");
	return string("\"")+cont+"\"";
}

void PythonExporter::comentar (t_output & prog, string text, std::string tabs) {
	if (!for_test) insertar(prog,tabs+"# "+text);
}
