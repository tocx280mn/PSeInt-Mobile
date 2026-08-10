/**
* @file export_vb.cpp
*
* Las traducciones están basadas en los ejemplos enviados por el Prof. Tito Sánchez
**/
#include "version.h"
#include "exportexp.h"
#include "ExporterBase.hpp"
#include "TiposExporter.hpp"
#include "VBExporter.hpp"

using namespace std;

VbExporter::VbExporter() {
	output_base_zero_arrays=false;
}

void VbExporter::esperar_tecla(t_output &prog, std::string tabs){
	insertar(prog,tabs+"Console.ReadKey()");
}

void VbExporter::esperar_tiempo(t_output &prog, string tiempo, bool milis, std::string tabs){
	tipo_var t; tiempo=expresion(tiempo,t); // para que arregle los nombres de las variables
	stringstream inst;
	inst<<"Thread.Sleep(";
	if (milis) inst<<tiempo; 
	else {
		inst<<colocarParentesis(tiempo)<<"*1000";
	}
	inst<<")";
	insertar(prog,tabs+inst.str());
}

void VbExporter::borrar_pantalla(t_output &prog, std::string tabs) {
	insertar(prog,tabs+"Console.Clear()");
}

void VbExporter::invocar(t_output &prog, std::string func_name, std::string args, std::string tabs){
	if (args.empty()) args = "()";
	string linea = expresion(func_name+args);
	insertar(prog,tabs+linea);
}

void VbExporter::escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs){
	t_arglist_it it=args.begin();
	string linea;
	while (it!=args.end()) {
		if (linea.size()) linea+=",";
		linea+=expresion(*it);
		++it;
	}
	if (saltar) linea=string("Console.WriteLine(")+linea+")"; 
	else linea=string("Console.Write(")+linea+")";
	insertar(prog,tabs+linea);
}

void VbExporter::leer(t_output &prog, t_arglist args, std::string tabs){
	t_arglist_it it=args.begin();
	while (it!=args.end()) {
		tipo_var t;
		string varname=expresion(*it,t);
		if (t==vt_numerica && t.rounded) insertar(prog,tabs+varname+" = Integer.Parse(Console.ReadLine())");
		else if (t==vt_numerica) insertar(prog,tabs+varname+" = Double.Parse(Console.ReadLine())");
		else if (t==vt_logica) insertar(prog,tabs+varname+" = Boolean.Parse(Console.ReadLine())");
		else  insertar(prog,tabs+varname+" = Console.ReadLine()");
		++it;
	}
}

void VbExporter::asignacion(t_output &prog, string param1, string param2, std::string tabs){
	insertar(prog,tabs+expresion(param1)+" = "+expresion(param2));
}

void VbExporter::si(t_output &prog, t_proceso_it it_si, t_proceso_it it_sino, t_proceso_it it_fin, std::string tabs){
	insertar(prog,tabs+"If "+expresion(getImpl<IT_SI>(*it_si).condicion)+" Then");
	bloque(prog,std::next(it_si),it_sino,tabs+"\t");
	if (it_sino!=it_fin) {
		insertar(prog,tabs+"Else");
		bloque(prog,std::next(it_sino),it_fin,tabs+"\t");
	}
	insertar(prog,tabs+"End If");
}

void VbExporter::mientras(t_output &prog, t_proceso_it it_mientras, t_proceso_it it_fin, std::string tabs){
	insertar(prog,tabs+"While "+expresion(getImpl<IT_MIENTRAS>(*it_mientras).condicion));
	bloque(prog,std::next(it_mientras),it_fin,tabs+"\t");
	insertar(prog,tabs+"End While");
}

void VbExporter::segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs){
	
	insertar(prog,tabs+"Select Case "+expresion(getImpl<IT_SEGUN>(*(its[0])).expresion));
	for (size_t i=1; i+1<its.size(); ++i) {
		if (its[i]->type==IT_DEOTROMODO)
			insertar(prog,tabs+"Case Else");
		else {
			auto &exps = getImpl<IT_OPCION>(*(its[i])).expresiones;
			string case_line;
			for(size_t j=0;j<exps.size();++j)
				case_line += (j==0?"Case ":", ") + expresion(exps[j]);
//			bool comillas=false; int parentesis=0, j=0,l=case_line.size();
//			while(j<l) {
//				if (case_line[j]=='\''||case_line[j]=='\"') comillas=!comillas;
//				else if (!comillas) {
//					if (case_line[j]=='['||case_line[j]=='(') parentesis++;
//					else if (case_line[j]==']'||case_line[j]==')') parentesis--;
//					else if (parentesis==0 && case_line[j]==',') {
//						case_line.replace(j,1,", "); l+=6;
//					}
//				}
//				j++;
//			}
			insertar(prog,tabs+case_line);
		}
		bloque(prog,std::next(its[i]),its[i+1],tabs+"\t");
	}
	insertar(prog,tabs+"End Select");
}

void VbExporter::repetir(t_output &prog, t_proceso_it it_repetir, t_proceso_it it_hasta, std::string tabs){
	insertar(prog,tabs+"Do");
	bloque(prog,std::next(it_repetir),it_hasta,tabs+"\t");
	auto &impl = getImpl<IT_HASTAQUE>(*it_hasta);
	if (impl.mientras_que)
		insertar(prog,tabs+"Loop While "+expresion(impl.condicion));
	else
		insertar(prog,tabs+"Loop Until "+expresion(impl.condicion));
}

void VbExporter::para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs){
	auto &impl = getImpl<IT_PARA>(*it_para);
	std::string var = expresion(impl.contador), ini = expresion(impl.val_ini),
		        fin = expresion(impl.val_fin), paso = impl.paso;
	if (paso=="1" or paso.empty())
		insertar(prog,tabs+"For "+var+"="+ini+" To "+fin);
	else
		insertar(prog,tabs+"For "+var+"="+ini+" To "+fin+" Step "+expresion(paso));
	bloque(prog,std::next(it_para),it_fin,tabs+"\t");
	insertar(prog,tabs+"Next "+var);
}

void VbExporter::paracada(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs){
	auto &impl = getImpl<IT_PARACADA>(*it_para);
	std::string arreglo = ToLower(impl.arreglo), identif = ToLower(impl.identificador);
	insertar(prog,tabs+"For Each "+identif+" In "+arreglo);
	bloque(prog,++it_para,it_fin,tabs+"\t");
	insertar(prog,tabs+"Next");
	memoria->RemoveVar(identif);
}

string VbExporter::function(string name, string args) {
	if (name=="SEN") {
		return string("Math.Sin")+args;
	} else if (name=="TAN") {
		return string("Math.Tan")+args;
	} else if (name=="ASEN") {
		return string("Math.Asin")+args;
	} else if (name=="ACOS") {
		return string("Math.Acos")+args;
	} else if (name=="COS") {
		return string("Math.Cos")+args;
	} else if (name=="RAIZ") {
		return string("Math.Sqrt")+args;
	} else if (name=="RC") {
		return string("Math.Sqrt")+args;
	} else if (name=="ABS") {
		return string("Math.Abs")+args;
	} else if (name=="LN") {
		return string("Math.Log")+args;
	} else if (name=="EXP") {
		return string("Math.Exp")+args;
	} else if (name=="AZAR") {
		return string("New Random().Next")+args;
	} else if (name=="ATAN") {
		return string("Math.Atan")+args;
	} else if (name=="TRUNC") {
		return string("Math.Truncate")+args;
	} else if (name=="REDON") {
		return string("Math.Round")+args;
	} else if (name=="CONCATENAR") {
		return get_arg(args,1)+" & "+get_arg(args,2);
	} else if (name=="LONGITUD") {
		return get_arg(args,1)+".Length()";
	} else if (name=="SUBCADENA") {
		if (!input_base_zero_arrays) args=
			string("(")+get_arg(args,1)+","
			+sumarOrestarUno(get_arg(args,2),false)
			+","+sumarOrestarUno(get_arg(args,3),false)+")";
		return string("Mid")+args;
	} else if (name=="CONVERTIRANUMERO") {
		return string("CDbl")+args;
	} else if (name=="CONVERTIRATEXTO") {
		return string("CStr")+args;
	} else if (name=="MINUSCULAS") {
		return get_arg(args,1)+".ToUpper()";
	} else if (name=="MAYUSCULAS") {
		return get_arg(args,1)+".ToLower()";
	} else {
		return ToLower(name)+args; // no deberia pasar esto
	}
}

// funcion usada por declarar_variables para las internas de una funcion
// y para obtener los tipos de los argumentos de la funcion para las cabeceras
string VbExporter::get_tipo(map<string,tipo_var>::iterator &mit, bool for_func, bool by_ref) {
	tipo_var &t=mit->second;
	string stipo="String";
	if (t==vt_caracter) { stipo="String"; }
	else if (t==vt_numerica) stipo=t.rounded?"Integer":"Double";
	else if (t==vt_logica) stipo="Boolean";
	//else use_sin_tipo=true;
	if (t.dims) {
		if (!for_func) return "";
		string pre=for_func?"ByVal ":"Dim ";
		return pre+ToLower(mit->first)+make_dims(t.dims,"(",",",")",!for_func)+" As "+stipo;
	} else {
		string pre=for_func?(by_ref?"ByRef ":"ByVal "):"Dim ";
		return pre+ToLower(mit->first)+" As "+stipo;
	}
}

// resolucion de tipos (todo lo que acceda a cosas privadas de memoria tiene que estar en esta clase porque es la unica amiga)
void VbExporter::declarar_variables(t_output &prog) {
	map<string,tipo_var>::iterator mit=memoria->GetVarInfo().begin(), mit2=memoria->GetVarInfo().end();
	string tab("\t\t");
	while (mit!=mit2) {
		string dec=get_tipo(mit);
		if (dec.size()) prog.push_back(tab+dec);
		++mit;
	}
}

// retorna el tipo y elimina de la memoria a esa variable
// se usa para armar las cabeceras de las funciones, las elimina para que no se
// vuelvan a declarar adentro
string VbExporter::get_tipo(string name, bool by_ref) {
	map<string,tipo_var>::iterator mit=memoria->GetVarInfo().find(name);
	if (mit==memoria->GetVarInfo().end()) 
		return string("Dim ")+ToLower(name)+" As String"; // puede pasar si hay variables que no se usan dentro de la funcion
	string ret = get_tipo(mit,true,by_ref);
	memoria->GetVarInfo().erase(mit);
	return ret;
}

void VbExporter::translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) {
	
	//cuerpo del proceso
	t_output out_proc;
	bloque(out_proc,++proc.begin(),proc.end(),"\t\t");
	
	// cabecera del proceso
	bool is_sub=true;
	string ret; // sentencia "Return ..." de la funcion
	if (!f) {
		out.push_back("\tSub Main()");
	} else {
		string dec;
		if (f->nombres[0]=="") {
			dec="\tPublic Sub "; 
		} else {
			is_sub=false;
			dec="\tPublic Function ";
			ret=string("\tReturn ")+ToLower(f->nombres[0]);
		}
		dec+=ToLower(f->id)+"(";
		for(int i=1;i<=f->GetArgsCount();i++) {
			if (i!=1) dec+=", ";
			dec+=get_tipo(f->nombres[i],f->pasajes[i]==PP_REFERENCIA);
		}
		dec+=")";
		out.push_back(dec);
	}
	
	declarar_variables(out);
	
	insertar_out(out,out_proc);
	
	// cola del proceso
	if (ret.size()) out.push_back(string("\t")+ret);
	out.push_back(is_sub?"\tEnd Sub":"\tEnd Function");
	if (!for_test) out.push_back("");
	
}

void VbExporter::translate(t_output &out, Programa &prog) {
	
	load_subs_in_funcs_manager(prog);
	
	// cppcheck-suppress unusedScopedObject
	TiposExporter(prog,false); // para que se cargue el mapa_memorias con memorias que tengan ya definidos los tipos de variables que correspondan
	
	// cabecera
	init_header(out,"' ");
	out.push_back(string("Module ")+GetRT().funcs.GetMainName());
	if (!for_test) out.push_back("");
	// procesos y subprocesos
	translate_all_procs(out,prog,"\t");
	out.push_back("End Module");
}

string VbExporter::get_constante(string name) {
	if (name=="PI") return "Math.Pi";
	if (name=="VERDADERO") return "True";
	if (name=="FALSO") return "False";
	return name;
}

string VbExporter::get_operator(string op, bool for_string) {
	// para agrupar operaciones y alterar la jerarquia
	if (op=="(") return "("; 
	if (op==")") return ")";
	// para llamadas a funciones
	if (op=="{") return "("; 
	if (op=="}") return ")";
	// para indices de arreglos
	if (op=="[") return "(";
	if (op==",") return ",";
	if (op=="]") return ")";
	// otros
	if (for_string) {
		if (op=="+") return "&"; 
		if (op=="=") return "func arg1.Equals(arg2)"; 
		if (op=="<>") return "func Not arg1.Equals(arg2)"; 
		if (op=="<") return "func arg1.CompareTo(arg2)<0"; 
		if (op==">") return "func arg1.CompareTo(arg2)>0"; 
		if (op=="<=") return "func arg1.CompareTo(arg2)<=0"; 
		if (op==">=") return "func arg1.CompareTo(arg2)>=0"; 
	}
	if (op=="+") return "+"; 
	if (op=="-") return "-"; 
	if (op=="/") return "/"; 
	if (op=="*") return "*";
	if (op=="^") return "^";
	if (op=="%") return " Mod ";
	if (op=="=") return "="; 
	if (op=="<>") return "<>"; 
	if (op=="<") return "<"; 
	if (op==">") return ">"; 
	if (op=="<=") return "<="; 
	if (op==">=") return ">="; 
	if (op=="&") return " And "; 
	if (op=="|") return " Or "; 
	if (op=="~") return "Not "; 
	return op; // no deberia pasar nunca
}

string VbExporter::make_string (string cont) {
	return string("\"")+cont+"\"";
}

void VbExporter::dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs) {
	for(size_t i=0;i<nombres.size();++i) { 
		tipo_var t = memoria->LeerTipo(nombres[i]);
		string stipo="String";
		if      (t==vt_caracter) stipo="String";
		else if (t==vt_numerica) stipo=t.rounded?"Integer":"Double";
		else if (t==vt_logica)   stipo="Boolean";
		std::string dims = "("+tamanios[i];
		if (dims[dims.size()-1]==',') dims[dims.size()-1] = ')'; else dims += ")";
		insertar(prog,tabs+"Dim "+expresion(nombres[i]+dims)+" As "+stipo);
	}
}

void VbExporter::comentar (t_output & prog, string text, std::string tabs) {
	if (!for_test) insertar(prog,tabs+"' "+text);
}
