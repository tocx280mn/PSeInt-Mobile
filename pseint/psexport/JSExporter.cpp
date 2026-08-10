#include "JSExporter.hpp"
#include "exportexp.h"

using namespace std;

JSExporter::JSExporter(bool for_html):CppExporter() {
	this->for_html=for_html;
}

void JSExporter::borrar_pantalla(t_output &prog, std::string tabs) {
	insertar(prog,tabs+"document.body.innerHTML = \'\';");
}

void JSExporter::esperar_tecla(t_output &prog, std::string tabs) {
	if (for_test)
		insertar(prog,tabs+"prompt();");
	else
		insertar(prog,tabs+"prompt(); // no hay una forma simple de esperar una tecla en javascript sin separar esta función (vease document.onKeyUp), lo cual no siempre es posible");
}

void JSExporter::esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) {
	if (for_test)
		insertar(prog,tabs+"prompt();");
	else
		insertar(prog,tabs+"prompt(); // no hay una forma simple de esperar un tiempo en javascript sin separar esta función (vease setTimeOut), lo cual no siempre es posible");
}

void JSExporter::escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs){
	string arglist;
	t_arglist_it it=args.begin();
	while (it!=args.end()) {
		if (arglist.size()) arglist+=", ";
		tipo_var t;
		arglist+=expresion(*it,t);
		++it;
	}
	if (saltar) arglist+=", \'<BR/>\'";
	insertar(prog,tabs+"document.write("+arglist+");");
}

void JSExporter::leer(t_output &prog, t_arglist args, std::string tabs) {
	t_arglist_it it=args.begin();
	while (it!=args.end()) {
		tipo_var t;
		string varname=expresion(*it,t);
		if (t==vt_numerica && t.rounded) insertar(prog,tabs+varname+" = Number(prompt());");
		else if (t==vt_numerica) insertar(prog,tabs+varname+" = Number(prompt());");
		else if (t==vt_logica) insertar(prog,tabs+varname+" = Boolean(prompt());");
		else { read_strings=true; insertar(prog,tabs+varname+" = prompt();"); }
		++it;
	}
	
}

void JSExporter::paracada(t_output &out, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) {
	auto &impl = getImpl<IT_PARACADA>(*it_para);
	std::string arreglo = ToLower(impl.arreglo), identif = ToLower(impl.identificador);
	const int *dims = memoria->LeerDims(arreglo);
	if (!dims) { insertar(out,string("ERROR: ")+arreglo+" NO ES UN ARREGLO"); return; }
	int n=dims[0];
	
	string *auxvars=new string[n];
	for(int i=0;i<n;i++) auxvars[i]=get_aux_varname("aux_index_");
	
	string vname=arreglo;
	for(int i=0;i<n;i++) { 
		string idx=auxvars[i];
		insertar(out,tabs+"for (var "+idx+"=0; "+idx+"<"+vname+".length; ++"+idx+") {");
		vname+="["+idx+"]";
		tabs+="\t";
	}
	
	for(int i=n-1;i>=0;i--) release_aux_varname(auxvars[i]);
	delete []auxvars;
	
	t_output aux_out;
	bloque(aux_out,std::next(it_para),it_fin,tabs);
	replace_var(aux_out,identif,vname);
	insertar_out(out,aux_out);
	for(int i=0;i<n;i++) { 
		tabs.erase(tabs.size()-1);
		insertar(out,tabs+"}");
	}
	memoria->RemoveVar(identif);
}

string JSExporter::function(string name, string args) {
	if (name=="SEN") {
		return string("Math.sin")+args;
	} else if (name=="TAN") {
		return string("Math.tan")+args;
	} else if (name=="ASEN") {
		return string("Math.asin")+args;
	} else if (name=="ACOS") {
		return string("Math.acos")+args;
	} else if (name=="COS") {
		return string("Math.cos")+args;
	} else if (name=="RAIZ") {
		return string("Math.sqrt")+args;
	} else if (name=="RC") {
		return string("Math.sqrt")+args;
	} else if (name=="ABS") {
		return string("Math.abs")+args;
	} else if (name=="LN") {
		return string("Math.log")+args;
	} else if (name=="EXP") {
		return string("Math.exp")+args;
	} else if (name=="AZAR") {
		return string("Math.floor(Math.random()*")+colocarParentesis(get_arg(args,1))+")";
	} else if (name=="ATAN") {
		return string("Math.atan")+args;
	} else if (name=="TRUNC") {
		return string("Math.trunc")+args;
	} else if (name=="REDON") {
		return string("Math.round")+args;
	} else if (name=="CONCATENAR") {
		return string("String.concat")+args;
	} else if (name=="MAYUSCULAS") {
		return string("String.toUpperCase")+args;
	} else if (name=="MINUSCULAS") {
		return string("String.toLowerCase")+args;
	} else if (name=="LONGITUD") {
		return get_arg(args,1)+".length";
	} else if (name=="SUBCADENA") {
		string desde=get_arg(args,2);
		if (!input_base_zero_arrays) desde=sumarOrestarUno(desde,false);
		string hasta=get_arg(args,3);
		if (input_base_zero_arrays) hasta=sumarOrestarUno(hasta,true);
		return string("String.substring(")+get_arg(args,1)+", "+desde+", "+hasta+")";
	} else if (name=="CONVERTIRATEXTO") {
		return string("String")+args;
	} else if (name=="CONVERTIRANUMERO") {
		return string("Number")+args;
	} else {
		return ToLower(name)+args; // no deberia pasar esto
	}
}

void JSExporter::header(t_output &out) {
	init_header(out,for_html?"\t\t\t//":"// ");
}

void JSExporter::footer(t_output &out) {
}


void JSExporter::translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) {
	
	//cuerpo del proceso
	t_output out_proc;
	string ret,tab=for_html?"\t\t\t":"";
	bloque(out_proc,++proc.begin(),proc.end(),tab+"\t");
	
	// cabecera del proceso
	if (!f) {
		insertar(out,tab+"function "+ToLower(GetRT().funcs.GetMainName())+"() {");
	} else {
		string dec=tab+"function ";
		if (f->nombres[0]!="") 
			ret=string("return ")+ToLower(f->nombres[0]);
		dec+=ToLower(f->id)+"(";
		for(int i=1;i<=f->GetArgsCount();i++) {
			if (i!=1) dec+=", ";
			string var_dec=ToLower(f->nombres[i]);
			memoria->RemoveVar(f->nombres[i]);
			dec+=var_dec;
		}
		dec+=") {";
		out.push_back(dec);
	}
	
	declarar_variables(out,tab+"\t");
	
	insertar_out(out,out_proc);
	
	// cola del proceso
	if (ret.size()) out.push_back(tab+"\t"+ret+";");
	out.push_back(tab+"}");
	if (!for_test) out.push_back(tab);
}

string JSExporter::get_operator(string op, bool for_string) {
	if (op=="^") { include_cmath=true; return "func Math.pow(arg1, arg2)"; }
	else return CppExporter::get_operator(op,false);
}

void JSExporter::dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs) {
	ExporterBase::dimension(prog,nombres,tamanios,tabs);
	for(size_t i=0;i<nombres.size();++i) {
		// obtener nombre y dimensiones
		std::string name = nombres[i], dims = tamanios[i];
		t_arglist dimlist = splitArgsList(dims);
		dims="";
		t_arglist_it it2=dimlist.begin();
		
		stack<string> auxs; string var=ToLower(name), last_size;
		string line=string("var ")+ToLower(name)+" = new Array("+(last_size=expresion(*it2))+");"; ++it2;
		while (it2!=dimlist.end()) {
			string aux=get_aux_varname("aux_index_"); auxs.push(aux);
			line+=" for (var "+aux+"=0; "+aux+"<"+last_size+"; ++"+aux+") { "; 
			var+="[";
			var+=aux;
			var+="]";
			line+=var+" = new Array("+(last_size=expresion(*it2))+");";
			++it2;
		}
		while(!auxs.empty()) { release_aux_varname(auxs.top()); auxs.pop(); line+=" }"; }
		
		// armar la linea que hace el new
		insertar(prog,tabs+line);
	}
}

string JSExporter::get_constante(string name) {
	if (name=="PI") { include_cmath=true; return "Math.PI"; }
	if (name=="VERDADERO") return "true";
	if (name=="FALSO") return "false";
	return name;
}


// resolucion de tipos (todo lo que acceda a cosas privadas de memoria tiene que estar en esta clase porque es la unica amiga)
void JSExporter::declarar_variables(t_output &prog, string tab) {
	string vars;
	map<string,tipo_var>::iterator mit=memoria->GetVarInfo().begin(), mit2=memoria->GetVarInfo().end();
	while (mit!=mit2) {
		if (!mit->second.defined && !mit->second.dims) {
			if (vars.size()) vars+=", ";
			vars+=ToLower(mit->first);
		}
		++mit;
	}
	if (vars.size()) prog.push_back(tab+"var "+vars+";");
}

void JSExporter::definir(t_output &prog, t_arglist &variables, tipo_var tipo, std::string tabs) {
	std::string str_tipo;
	if      (tipo==vt_numerica) str_tipo = "Number()";
	else if (tipo==vt_logica)   str_tipo="Boolean()";
	else if (tipo==vt_caracter) str_tipo="String()";
	else                        str_tipo = "Number()";
	for(auto &var : variables) {
		string vname = expresion(var);
		insertar(prog,tabs+"var "+vname+" = new "+str_tipo+";");
	}
}

void JSExporter::translate_all_procs (t_output &out, Programa &prog, std::string tabs) {
	ExporterBase::translate_all_procs(out,prog,for_html?"\t\t\t":"");
}
