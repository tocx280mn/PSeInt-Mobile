#include <sstream>
#include <cstdlib>
#include "CppExporter.hpp"
#include "version.h"
#include "exportexp.h"
#include "ExporterBase.hpp"
#include "TiposExporter.hpp"
using namespace std;

CppExporter::CppExporter() {
	output_base_zero_arrays=true;
}

void CppExporter::borrar_pantalla(t_output &prog, std::string tabs) {
	if (for_test)
		insertar(prog,tabs+"cout << endl;");
	else
		insertar(prog,tabs+"cout << endl; // no hay forma directa de borrar la pantalla con C++ estandar");
}

void CppExporter::esperar_tecla(t_output &prog, std::string tabs){
	if (for_test)
		insertar(prog,tabs+"cin.get();");
	else
		insertar(prog,tabs+"cin.get(); // a diferencia del pseudocódigo, espera un Enter, no cualquier tecla");
}

void CppExporter::esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) {
	tipo_var t; tiempo = expresion(tiempo,t); // para que arregle los nombres de las variables
	use_func_esperar=true;
	stringstream inst;
	inst<<"esperar(";
	if (mili) inst<<tiempo; 
	else {
		inst<<colocarParentesis(tiempo)<<"*1000";
	}
	inst<<");";
	insertar(prog,tabs+inst.str());
}

void CppExporter::invocar(t_output &prog, std::string func_name, std::string params, std::string tabs){
	std::string line = func_name+params;
	if (params.empty()) line += "()";
	insertar(prog,tabs+expresion(line)+";");
}

void CppExporter::escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs){
	t_arglist_it it=args.begin();
	string linea="cout";
	while (it!=args.end()) {
		linea+=" << ";
		tipo_var t;
		linea+=expresion(*it,t);
		if (t==vt_logica) prints_bool=true;
		++it;
	}
	insertar(prog,tabs+linea+(saltar?" << endl;":";"));
}

void CppExporter::leer(t_output &prog, t_arglist args, std::string tabs) {
	t_arglist_it it=args.begin();
	string linea="cin";
	while (it!=args.end()) {
		linea+=" >> ";
		tipo_var t;
		linea+=expresion(*it,t);
		if (t==vt_caracter) read_strings=true;
		++it;
	}
	insertar(prog,tabs+linea+";");
}

void CppExporter::asignacion(t_output &prog, string variable, string valor, std::string tabs){
	insertar(prog,tabs+expresion(variable)+" = "+expresion(valor)+";");
}

void CppExporter::si(t_output &prog, t_proceso_it it_si, t_proceso_it it_sino, t_proceso_it it_fin, std::string tabs){
	string condicion = expresion(getImpl<IT_SI>(*it_si).condicion);
	insertar(prog,tabs+"if ("+condicion+") {");
	bloque(prog,++it_si,it_sino,tabs+"\t");
	if (it_sino!=it_fin) {
		insertar(prog,tabs+"} else {");
		bloque(prog,++it_sino,it_fin,tabs+"\t");
	}
	insertar(prog,tabs+"}");
}

void CppExporter::mientras(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) {
	auto impl = getImpl<IT_MIENTRAS>(*r);
	insertar(prog,tabs+"while ("+expresion(impl.condicion)+") {");
	bloque(prog,++r,q,tabs+"\t");
	insertar(prog,tabs+"}");
}

void CppExporter::segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) {
	auto impl = getImpl<IT_SEGUN>(*(its[0]));
	insertar(prog,tabs+"switch ("+expresion(impl.expresion)+") {");
	for(size_t i=2;i<its.size();++i) {
		auto it_op = its[i-1], it_next = its[i];
		bool dom = it_op->type==IT_DEOTROMODO;
		if (dom) {
			insertar(prog,tabs+"default:");
		} else {
			for(std::string &exp : getImpl<IT_OPCION>(*it_op).expresiones)
				insertar(prog,tabs+"case "+expresion(exp)+":");
		}
		bloque(prog,++it_op,it_next,tabs+"\t");
		if (!dom) insertar(prog,tabs+"\tbreak;");
	}
	insertar(prog,tabs+"}");
}

void CppExporter::repetir(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs){
	auto impl = getImpl<IT_HASTAQUE>(*q);
	insertar(prog,tabs+"do {");
	bloque(prog,++r,q,tabs+"\t");
	if (impl.mientras_que)
		insertar(prog,tabs+"} while ("+expresion(impl.condicion)+");");
	else
		insertar(prog,tabs+"} while ("+expresion(invert_expresion(impl.condicion)+");"));
}

void CppExporter::para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs){
	auto &impl = getImpl<IT_PARA>(*it_para);
	string var = expresion(impl.contador), ini = expresion(impl.val_ini), 
		   fin = expresion(impl.val_fin), paso = impl.paso;
	if ((not paso.empty()) and paso[0]=='-') {
		if (paso=="-1")
			insertar(prog,tabs+"for ("+var+"="+ini+"; "+var+">="+fin+"; --"+var+") {");
		else
			insertar(prog,tabs+"for ("+var+"="+ini+"; "+var+">="+fin+"; "+var+"-="+expresion(paso.substr(1))+") {");
	} else {
		if (paso.empty() or paso=="1")
			insertar(prog,tabs+"for ("+var+"="+ini+"; "+var+"<="+fin+"; ++"+var+") {");
		else
			insertar(prog,tabs+"for ("+var+"="+ini+"; "+var+"<="+fin+"; "+var+"+="+expresion(paso)+") {");
	}
	bloque(prog,std::next(it_para),it_fin,tabs+"\t");
	insertar(prog,tabs+"}");
}

void CppExporter::paracada(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs){
	auto &impl = getImpl<IT_PARACADA>(*it_para);
	string identif = ToLower(impl.identificador), arreglo = ToLower(impl.arreglo);
	string first = arreglo, last = arreglo;
	const int *dims = memoria->LeerDims(arreglo);
	if (!dims) { insertar(prog,string("Error: ")+arreglo+" no es un arreglo"); return; }
	for (int i=1;i<=dims[0];i++) {
		first+="[0]";
		last+="[";
		last+=IntToStr(dims[i]-1);
		last+="]";
	}
	string ptr = get_aux_varname("aux_ptr_");
	insertar(prog,tabs+"for (auto "+ptr+"=&("+first+"); "+ptr+"<=&("+last+"); ++"+ptr+") {");
	insertar(prog,tabs+"\tauto &"+identif+" = *"+ptr+";");
	bloque(prog,std::next(it_para),it_fin,tabs+"\t");
	memoria->RemoveVar(identif);
	insertar(prog,tabs+"}");
	release_aux_varname(ptr);
}


string CppExporter::convertirAString(const string &s) {
	if (es_cadena_constante(s)) return string("string("+s+")");
	else return s;
}

string CppExporter::function(string name, string args) {
	if (name=="SEN") {
		include_cmath=true;
		return string("sin")+args;
	} else if (name=="TAN") {
		include_cmath=true;
		return string("tan")+args;
	} else if (name=="ASEN") {
		include_cmath=true;
		return string("asin")+args;
	} else if (name=="ACOS") {
		include_cmath=true;
		return string("acos")+args;
	} else if (name=="COS") {
		include_cmath=true;
		return string("cos")+args;
	} else if (name=="RAIZ") {
		include_cmath=true;
		return string("sqrtf")+args;
	} else if (name=="RC") {
		include_cmath=true;
		return string("sqrtf")+args;
	} else if (name=="ABS") {
		include_cmath=true;
		return string("abs")+args;
	} else if (name=="LN") {
		include_cmath=true;
		return string("log")+args;
	} else if (name=="EXP") {
		include_cmath=true;
		return string("exp")+args;
	} else if (name=="AZAR") {
		include_cstdlib=true;
		return string("(rand()%")+colocarParentesis(get_arg(args,1))+")";
	} else if (name=="ALEATORIO") {
		include_cstdlib=true;
		string diff = sumarOrestarUno(colocarParentesis(get_arg(args,2))+"-"+colocarParentesis(get_arg(args,1)),true);
		return string("(")+get_arg(args,1)+"+rand()%"+colocarParentesis(diff)+")";
	} else if (name=="ATAN") {
		include_cmath=true;
		return string("atan")+args;
	} else if (name=="TRUNC") {
		include_cmath=true;
		return string("int")+args;
	} else if (name=="REDON") {
		include_cmath=true;
		return string("int(")+colocarParentesis(get_arg(args,1))+"+.5)";
	} else if (name=="CONCATENAR") {
		return string("(")+convertirAString(get_arg(args,1))+"+"+get_arg(args,2)+")";
	} else if (name=="LONGITUD") {
		return convertirAString(get_arg(args,1))+".size()";
	} else if (name=="SUBCADENA") {
		string desde = get_arg(args,2);
		string cuantos = sumarOrestarUno(get_arg(args,3)+"-"+get_arg(args,2),true);
		if (!input_base_zero_arrays) desde = sumarOrestarUno(desde,false);
		return convertirAString(get_arg(args,1))+".substr("+desde+", "+cuantos+")";
	} else if (name=="CONVERTIRANUMERO") {
		include_cstdlib=true;
		string s=get_arg(args,1);
		if (es_cadena_constante(s)) return string("atof(")+s+")";
		else return string("atof(")+colocarParentesis(s)+".c_str())";
	} else {
		if (name=="MINUSCULAS") use_func_minusculas=true;
		if (name=="MAYUSCULAS") use_func_mayusculas=true;
		if (name=="CONVERTIRATEXTO") use_func_convertiratexto=true;
		return ToLower(name)+args; // no deberia pasar esto
	}
}

// funcion usada por declarar_variables para las internas de una funcion
// y para obtener los tipos de los argumentos de la funcion para las cabeceras
string CppExporter::get_tipo(map<string,tipo_var>::iterator &mit, bool for_func, bool by_ref) {
	tipo_var &t=mit->second;
	string stipo="SIN_TIPO ";
	if (t==vt_caracter) { stipo="string "; use_string=true; }
	else if (t==vt_numerica) stipo=t.rounded?"int ":"float ";
	else if (t==vt_logica) stipo="bool ";
	else use_sin_tipo=true;
	if (t.dims) {
		string ret=stipo+ToLower(mit->first)+make_dims(t.dims,"[","][","]",!for_func);
		while (ret.find("[0]")!=string::npos) {
			use_arreglo_max=true;
			ret.replace(ret.find("[0]"),3,"[ARREGLO_MAX]");
		}
		return ret;
	} else {
		if (by_ref) stipo+="&";
		return stipo+ToLower(mit->first);
	}
}

// resolucion de tipos (todo lo que acceda a cosas privadas de memoria tiene que estar en esta clase porque es la unica amiga)
void CppExporter::declarar_variables(t_output &prog, string tab, bool ignore_arrays) {
	map<string,tipo_var>::iterator mit=memoria->GetVarInfo().begin(), mit2=memoria->GetVarInfo().end();
	while (mit!=mit2) {
		if (!ignore_arrays||!mit->second.dims)
			prog.push_back(tab+get_tipo(mit)+";");
		++mit;
	}
}

// retorna el tipo y elimina de la memoria a esa variable
// se usa para armar las cabeceras de las funciones, las elimina para que no se
// vuelvan a declarar adentro
string CppExporter::get_tipo(string name, bool by_ref, bool do_erase) {
	map<string,tipo_var>::iterator mit=memoria->GetVarInfo().find(name);
	if (mit==memoria->GetVarInfo().end()) 
		return "SIN_TIPO "+ToLower(name); // puede pasar si hay variables que no han sido usadas dentro de la funcion
	string ret = get_tipo(mit,true,by_ref);
	if (do_erase) memoria->GetVarInfo().erase(mit);
	return ret;
}

void CppExporter::header(t_output &out) {
	// cabecera
	init_header(out,"// ");
	out.push_back("#include<iostream>");
	if (include_cmath) out.push_back("#include<cmath>");
	if (include_cstdlib) out.push_back("#include<cstdlib>");
	if (use_func_esperar) out.push_back("#include<ctime>");
	if (use_func_mayusculas||use_func_minusculas) out.push_back("#include<cctype>");
	if (use_func_convertiratexto) out.push_back("#include<sstream>");
	out.push_back("using namespace std;");
	if (!for_test) out.push_back("");
	if (use_func_esperar) {
		if (!for_test) out.push_back("// No hay en el C++ estandar una funcion equivalente a \"esperar\", pero puede programarse una similar");
		out.push_back("void esperar(double t);");
		if (!for_test) out.push_back("");
	}
	if (use_func_convertiratexto) {
		if (!for_test) out.push_back("// No hay en el C++ estandar una funcion equivalente a \"convertiratexto\", pero puede programarse una equivalente.");
		out.push_back("string convertiratexto(float f);");
		if (!for_test) out.push_back("");
	}
	if (use_func_mayusculas) {
		if (!for_test) out.push_back("// No hay en el C++ estandar una funcion equivalente a \"mayusculas\", pero puede programarse una equivalente.");
		out.push_back("string mayusculas(string s);");
		if (!for_test) out.push_back("");
	}
	if (use_func_minusculas) {
		if (!for_test) out.push_back("// No hay en el C++ estandar una funcion equivalente a \"minusculas\", pero puede programarse una equivalente.");
		out.push_back("string minusculas(string s);");
		if (!for_test) out.push_back("");
	}
	if (use_arreglo_max) {
		if (!for_test) {
			out.push_back("// En C++ no se puede dimensionar un arreglo estático con una dimensión no constante.");
			out.push_back("// PSeInt sobredimensionará el arreglo utilizando un valor simbólico ARREGLO_MAX.");
			out.push_back("// Sería posible crear un arreglo dinámicamente con los operadores new y delete, pero");
			out.push_back("// este mecanismo aún no está soportado en las traducciones automáticas de PSeInt.");
		}
		out.push_back("#define ARREGLO_MAX 100");
		if (!for_test) out.push_back("");
	}
	if (use_sin_tipo) {
		if (!for_test) {
			out.push_back("// Para las variables que no se pudo determinar el tipo se utiliza la constante");
			out.push_back("// SIN_TIPO. El usuario debe reemplazar sus ocurrencias por el tipo adecuado");
			out.push_back("// (usualmente int,float,string o bool).");
		}
		out.push_back("using SIN_TIPO = string;");
		if (!for_test) out.push_back("");
	}
	if (read_strings) {
		if (!for_test) {
			out.push_back("// Para leer variables de texto se utiliza el operador << del objeto cin, que");
			out.push_back("// lee solo una palabra. Para leer una linea completa (es decir, incluyendo los");
			out.push_back("// espacios en blanco) se debe utilzar getline (ej, reemplazar cin>>x por");
			out.push_back("// getline(cin,x)), pero obliga a agregar un cin.ignore() si antes del getline");
			out.push_back("// se leyó otra variable con >>.");
			out.push_back("");
		}
	}
}

void CppExporter::footer(t_output &out) {
	if (use_func_esperar) {
		if (!for_test) out.push_back("");
		out.push_back("void esperar(double t) {");
		out.push_back("\tclock_t t0 = clock();");
		out.push_back("\tdouble e = 0;");
		out.push_back("\tdo {");
		out.push_back("\t\te = 1000*double(clock()-t0)/CLOCKS_PER_SEC;");
		out.push_back("\t} while (e<t);");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
	if (use_func_convertiratexto) {
		if (!for_test) out.push_back("");
		out.push_back("string convertiratexto(float f) {");
		out.push_back("\tstringstream ss;");
		out.push_back("\tss << f;");
		out.push_back("\treturn ss.str();");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
	if (use_func_mayusculas) {
		if (!for_test) out.push_back("");
		out.push_back("string mayusculas(string s) {");
		out.push_back("\tfor (unsigned int i=0; i<s.size(); ++i)");
		out.push_back("\t\ts[i] = toupper(s[i]);");
		out.push_back("\treturn s;");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
	if (use_func_minusculas) {
		if (!for_test) out.push_back("");
		out.push_back("string minusculas(string s) {");
		out.push_back("\tfor (unsigned int i=0; i<s.size(); ++i)");
		out.push_back("\t\ts[i] = tolower(s[i]);");
		out.push_back("\treturn s;");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
}


void CppExporter::translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) {
	
	// cuerpo del proceso
	t_output out_proc;
	bloque(out_proc,++proc.begin(),proc.end(),"\t");
	
	// cabecera del proceso
	string ret; // sentencia "return ..." de la funcion
	if (!f) {
		out.push_back("int main() {");
		if (prints_bool) {
			out.push_back("\tcout << boolalpha; // configurar cout para mostrar true/false en lugar de 0/1");
			if (!for_test) out.push_back("");
		}
		
		ret="return 0";
	} else {
		string dec;
		if (f->nombres[0].empty()) {
			dec="void "; 
		} else {
			ret=get_tipo(f->nombres[0],false,false);
			dec=ret.substr(0,ret.find(" ")+1);
			ret=string("return")+ret.substr(ret.find(" "));
		}
		dec+=ToLower(f->id)+"(";
		for(int i=1;i<=f->GetArgsCount();i++) {
			if (i!=1) dec+=", ";
			dec+=get_tipo(f->nombres[i],f->pasajes[i]==PP_REFERENCIA);
		}
		dec+=")";
		prototipos.push_back(dec+";");
		out.push_back(dec+" {");
	}
	
	declarar_variables(out);
	
	insertar_out(out,out_proc);
	
	// cola del proceso
	if (ret.size()) out.push_back(string("\t")+ret+";");
	out.push_back("}");
	if (!for_test) out.push_back("");
}

void CppExporter::translate(t_output &out, Programa &prog) {
	
	load_subs_in_funcs_manager(prog);
	
	// cppcheck-suppress unusedScopedObject
	TiposExporter(prog,true); // para que se cargue el mapa_memorias con memorias que tengan ya definidos los tipos de variables que correspondan
	
	t_output aux;
	translate_all_procs(aux,prog);
	
	header(out);
	if (!prototipos.empty()) {
		if (!for_test) {
			out.push_back("// Declaraciones adelantadas de las funciones");
			if (has_matrix_func) {
				out.push_back("// Las funciones que reciben arreglos en C/C++ deben especificar en sus");
				out.push_back("// prototipos las dimensiones de los mismos para todas las dimensiones,");
				out.push_back("// excepto la primera (que puede quedar vacía). Las funciones traducidas");
				out.push_back("// del pseudocódigo tienen todas sus dimensiones vacías, ya que PSeInt");
				out.push_back("// aún no determina automáticamente las mismas. Deberá completar las");
				out.push_back("// requeridas para poder compilar el programa.");
			}
		}
		insertar_out(out,prototipos);
		if (!for_test) out.push_back("");
	}
	insertar_out(out,aux);
	footer(out);
}

string CppExporter::get_constante(string name) {
	if (name=="PI") { include_cmath=true; return "M_PI"; }
	if (name=="VERDADERO") return "true";
	if (name=="FALSO") return "false";
	return name;
}

string CppExporter::get_operator(string op, bool for_string) {
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
	if (op=="^") { include_cmath=true; return "func pow(arg1, arg2)"; }
	if (op=="%") return "%";
	if (op=="=") return "=="; 
	if (op=="<>") return "!="; 
	if (op=="<") return "<"; 
	if (op==">") return ">"; 
	if (op=="<=") return "<="; 
	if (op==">=") return ">="; 
	if (op=="&") return " && "; 
	if (op=="|") return " || "; 
	if (op=="~") return "!"; 
	return op; // no deberia pasar nunca
}

string CppExporter::make_string (string cont) {
	for(unsigned int i=0;i<cont.size();i++)
		if (cont[i]=='\\') cont.insert(i++,"\\");
	return string("\"")+cont+"\"";
}

void CppExporter::comentar (t_output & prog, string text, std::string tabs) {
	if (!for_test) insertar(prog,tabs+"// "+text);
}

void CppExporter::translate_all_procs (t_output & out, Programa &prog, std::string tabs) {
	ExporterBase::translate_all_procs (out,prog,tabs);
}

