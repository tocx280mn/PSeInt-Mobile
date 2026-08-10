#include "exportexp.h"
#include "CSharpExporter.hpp"

using namespace std;

CSharpExporter::CSharpExporter():CppExporter() {
	use_threading=false;
	have_subprocesos=false;
	use_reference=true;
#ifdef DEBUG
	// solo para evitar warnings con cpp-check
	use_random=false;
#endif
}

void CSharpExporter::borrar_pantalla(t_output &prog, std::string tabs) {
	insertar(prog,tabs+"Console.Clear();");
}

void CSharpExporter::esperar_tecla(t_output &prog, std::string tabs) {
	insertar(prog,tabs+"Console.ReadKey();");
}

void CSharpExporter::esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) {
	tipo_var t; tiempo=expresion(tiempo,t); // para que arregle los nombres de las variables
	use_threading=true;
	stringstream inst;
	inst<<"Thread.Sleep(";
	if (mili) {
		inst<<tiempo; 
	} else {
		inst<<colocarParentesis(tiempo)<<"*1000";
	}
	inst<<");";
	insertar(prog,tabs+inst.str());
}

void CSharpExporter::escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs){
	string arglist;
	t_arglist_it it=args.begin();
	while (it!=args.end()) {
		if (arglist.size()) arglist+="+";
		tipo_var t;
		arglist+=expresion(*it,t);
		++it;
	}
	if (saltar)
		insertar(prog,tabs+"Console.WriteLine("+arglist+");");
	else
		insertar(prog,tabs+"Console.Write("+arglist+");");
}

void CSharpExporter::leer(t_output &prog, t_arglist args, std::string tabs) {
	t_arglist_it it=args.begin();
	while (it!=args.end()) {
		tipo_var t;
		string varname=expresion(*it,t);
		if (t==vt_numerica && t.rounded) insertar(prog,tabs+varname+" = int.Parse(Console.ReadLine());");
		else if (t==vt_numerica) insertar(prog,tabs+varname+" = Double.Parse(Console.ReadLine());");
		else if (t==vt_logica) insertar(prog,tabs+varname+" = Boolean.Parse(Console.ReadLine());");
		else { read_strings=true; insertar(prog,tabs+varname+" = Console.ReadLine();"); }
		++it;
	}
}

void CSharpExporter::paracada(t_output &out, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) {
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
		insertar(out,tabs+"for (int "+idx+"=0; "+idx+"<"+IntToStr(memoria->LeerDims(vname)[i+1])+"; ++"+idx+") {");
		vname+="["+idx+"]";
		tabs+="\t";
	}
	
	replace_all(vname,"][",",");
	
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

string CSharpExporter::function(string name, string args) {
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
		use_random=true;
		return string("azar.Next(0, ")+get_arg(args,1)+")";
	} else if (name=="ATAN") {
		return string("Math.Atan")+args;
	} else if (name=="TRUNC") {
		return string("Math.Truncate")+args;
	} else if (name=="REDON") {
		return string("Math.Round")+args;
	} else if (name=="CONCATENAR") {
		return get_arg(args,1)+"+"+get_arg(args,2);
	} else if (name=="MAYUSCULAS") {
		return get_arg(args,1)+".ToUpper()";
	} else if (name=="MINUSCULAS") {
		return get_arg(args,1)+".ToLower()";
	} else if (name=="LONGITUD") {
		return get_arg(args,1)+".Length";
	} else if (name=="SUBCADENA") {
		string desde=get_arg(args,2);
		string cuantos=sumarOrestarUno(get_arg(args,3)+"-"+get_arg(args,2),true);
		if (!input_base_zero_arrays) desde=sumarOrestarUno(desde,false);
		return get_arg(args,1)+".Substring("+desde+", "+cuantos+")";
	} else if (name=="CONVERTIRATEXTO") {
		return string("Convert.ToString")+args;
	} else if (name=="CONVERTIRANUMERO") {
		return string("Convert.ToDouble")+args;
	} else {
		return ToLower(name)+args; // no deberia pasar esto
	}
}

string CSharpExporter::get_tipo(map<string,tipo_var>::iterator &mit, bool for_func, bool by_ref) {
	string stipo=translate_tipo(mit->second)+" ";
	if (mit->second.dims) {
		return stipo+ToLower(mit->first)+make_dims(mit->second.dims,"[",",","]",false);
	} else {
		return stipo+ToLower(mit->first);
	}
}

string CSharpExporter::get_tipo(string name, bool by_ref, bool do_erase) {
	map<string,tipo_var>::iterator mit=memoria->GetVarInfo().find(name);
	if (mit==memoria->GetVarInfo().end()) 
		return "string "+ToLower(name); // puede pasar si hay variables que no han sido usadas dentro de la funcion
	
	tipo_var &t=mit->second;
	string stipo="string";
	if (t==vt_caracter) { stipo="string"; use_string=true; }
	else if (t==vt_numerica) stipo=t.rounded?"int":"double";
	else if (t==vt_logica) stipo="bool";
	else use_sin_tipo=true;
	if (by_ref) 
		stipo.insert(0,"ref ");
	if (t.dims) {
		stipo+=make_dims(t.dims,"[",",","]",false)+" "+ToLower(mit->first);
	} else {
		stipo+=" "; stipo+=ToLower(mit->first);
	}
	
	if (do_erase) memoria->GetVarInfo().erase(mit);
	return stipo;
}

void CSharpExporter::header(t_output &out) {
	// cabecera
	init_header(out,"// ");
	out.push_back("using System;");
	if (use_threading) out.push_back("using System.Threading;");
	if (!for_test) out.push_back("");
	out.push_back("namespace PSeInt {");
	out.push_back(string("\tclass ")+ToLower(GetRT().funcs.GetMainName())+" {");
	if (!for_test) out.push_back("");
}

void CSharpExporter::footer(t_output &out) {
	out.push_back("\t}"); // class
	if (!for_test) out.push_back("");
	out.push_back("}"); // namespace
	if (!for_test) out.push_back("");
}


void CSharpExporter::translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) {
	
	//cuerpo del proceso
	t_output out_proc;
	use_random=false;
	bloque(out_proc,++proc.begin(),proc.end(),"\t\t\t");
	
	// cabecera del proceso
	string ret; // sentencia "return ..." de la funcion
	if (!f) {
		out.push_back("\t\tstatic void Main(string[] args) {");
	} else {
		have_subprocesos=true;
		string dec="\t\tstatic ";
		if (f->nombres[0]=="") {
			dec+="void "; 
		} else {
			ret=get_tipo(f->nombres[0],false,false);
			dec+=ret.substr(0,ret.find(" ")+1);
			ret=string("return")+ret.substr(ret.find(" "));
		}
		dec+=ToLower(f->id)+"(";
		for(int i=1;i<=f->GetArgsCount();i++) {
			if (i!=1) dec+=", ";
			string var_dec=get_tipo(f->nombres[i],f->pasajes[i]==PP_REFERENCIA,true);
			if (f->pasajes[i]==PP_REFERENCIA) use_reference=true;
			dec+=var_dec;
		}
		dec+=") {";
		out.push_back(dec);
	}
	
	declarar_variables(out,"\t\t\t",true);
	if (use_random) insertar(out,"\t\t\tRandom azar = new Random();");
	insertar_out(out,out_proc);
	
	// cola del proceso
	if (ret.size()) out.push_back(string("\t\t\t")+ret+";");
	out.push_back("\t\t}");
	if (!for_test) out.push_back("");
}


string CSharpExporter::get_operator(string op, bool for_string) {
	if (for_string) {
		if (for_string) {
			if (op=="=") return "func arg1.Equals(arg2)"; 
			if (op=="<>") return "func !arg1.Equals(arg2)"; 
			if (op=="<") return "func arg1.CompareTo(arg2)<0"; 
			if (op==">") return "func arg1.CompareTo(arg2)>0"; 
			if (op=="<=") return "func arg1.CompareTo(arg2)<=0"; 
			if (op==">=") return "func arg1.CompareTo(arg2)>=0"; 
		}
	} else {
		if (op=="^") { return "func Math.Pow(arg1, arg2)"; }
		if (op==",") { return ","; }
	}
	return CppExporter::get_operator(op,false);
}


void CSharpExporter::dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs) {
	ExporterBase::dimension(prog,nombres,tamanios,tabs);
	for(size_t i=0;i<nombres.size();++i) {
		// obtener nombre y dimensiones
		std::string name = nombres[i], dims = tamanios[i];
		fix_dims(dims,"[",",","]");
		// armar la linea que hace el new
		string stipo = translate_tipo(memoria->LeerTipo(name));
		insertar(prog, tabs+stipo+make_dims(memoria->LeerDims(name),"[",",","]",false)+" "+ToLower(name)+" = new "+stipo+dims+";");
	}
}

string CSharpExporter::translate_tipo(const tipo_var &t) {
	if (t==vt_numerica) return t.rounded?"int":"double";
	if (t==vt_logica) return "bool";
	return "string";
}

string CSharpExporter::get_constante(string name) {
	if (name=="PI") return "Math.PI";
	if (name=="VERDADERO") return "true";
	if (name=="FALSO") return "false";
	return name;
}

void CSharpExporter::translate_all_procs (t_output & out, Programa & prog, std::string tabs) {
	ExporterBase::translate_all_procs(out,prog,"\t\t");
}

std::string CSharpExporter::referencia (const std::string & exp) {
	return "ref " + colocarParentesis(exp);
}

