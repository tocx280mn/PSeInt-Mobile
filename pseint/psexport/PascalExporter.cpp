#include <algorithm>
#include <sstream>
#include <cstdlib>
#include "ExporterBase.hpp"
#include "PascalExporter.hpp"
#include "version.h"
#include "exportexp.h"
#include "TiposExporter.hpp"
using namespace std;

#define linea_special_para_reemplazar_por_uses_y_otros "<{[!ACA_VAN_USES_TYPE_CONST!]}>"
#define linea_special_para_reemplazar_por_subprocesos "<{[!ACA_VAN_LOS_SUBPROCESOS!]}>"

PascalExporter::PascalExporter() {
	uses_randomize=false;
	uses_sin_tipo=false;
	uses_crt=false;
	uses_math=false;
	uses_sysutils=false;
	output_base_zero_arrays=input_base_zero_arrays;
	has_matrix_func=false;
	use_arreglo_max=false;
}

void PascalExporter::borrar_pantalla(t_output &prog, std::string tabs) {
	uses_crt=true;
	insertar(prog,tabs+"ClrScr;");
}

void PascalExporter::esperar_tecla(t_output &prog, std::string tabs){
	uses_crt=true;
	insertar(prog,tabs+"ReadKey;");
}

void PascalExporter::esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) {
	tipo_var t; tiempo=expresion(tiempo,t); // para que arregle los nombres de las variables
	uses_crt=true;
	stringstream inst;
	inst<<"Delay(";
	if (mili) inst<<tiempo; 
	else inst<<colocarParentesis(tiempo)<<"*1000";
	inst<<");";
	insertar(prog,tabs+inst.str());
}

void PascalExporter::invocar(t_output &prog, std::string func_name, std::string args, std::string tabs){
	if (args.empty()) args = "()";
	string linea = expresion(func_name+args) + ";";
	insertar(prog, tabs+linea);
}

void PascalExporter::escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs){
	t_arglist_it it=args.begin();
	string linea=saltar?"WriteLn":"Write", sep="(";
	while (it!=args.end()) {
		linea+=sep; sep=",";
		linea+=expresion(*it);
		++it;
	}
	linea+=");";
	insertar(prog,tabs+linea);
}

void PascalExporter::leer(t_output &prog, t_arglist args, std::string tabs) {
	t_arglist_it it=args.begin();
	string params, sep="(";
	while (it!=args.end()) {
		tipo_var t;
		string vname=expresion(*it,t);
		if (t==vt_logica) {
			memoria->DefinirTipo("aux_leer_logica",vt_caracter);
			if (params.size()) insertar(prog,tabs+"ReadLn"+params+");");
			if (for_test)
				insertar(prog,tabs+"ReadLn(aux_leer_logica);");
			else 
				insertar(prog,tabs+"ReadLn(aux_leer_logica); // ReadLn no puede leer booleanos, por eso se lee este string auxiliar y en la siguiente linea se convierte");
			insertar(prog,tabs+vname+" := (lowercase(aux_leer_logica)='verdadero');");
			sep="("; params="";
		} else {
			params+=sep+vname;
			sep=",";
		}
		++it;
	}
	if (params.size()) insertar(prog,tabs+"ReadLn"+params+");");
}

void PascalExporter::asignacion(t_output &prog, string param1, string param2, std::string tabs){
	insertar(prog,tabs+expresion(param1)+" := "+expresion(param2)+";");
}

void PascalExporter::si(t_output &prog, t_proceso_it it_si, t_proceso_it it_fin, t_proceso_it s, std::string tabs){
	insertar(prog, tabs+"If "+expresion(getImpl<IT_SI>(*it_si).condicion)+" Then Begin");
	bloque(prog, std::next(it_si), it_fin, tabs+"\t");
	if (it_fin!=s) {
		insertar(prog, tabs+"End");
		insertar(prog, tabs+"Else Begin");
		bloque(prog, std::next(it_fin), s, tabs+"\t");
	}
	insertar(prog, tabs+"End;");
}

void PascalExporter::mientras(t_output &prog, t_proceso_it it_mientras, t_proceso_it it_fin, std::string tabs){
	insertar(prog, tabs+"While "+expresion(getImpl<IT_MIENTRAS>(*it_mientras).condicion)+" Do Begin");
	bloque(prog, std::next(it_mientras), it_fin, tabs+"\t");
	insertar(prog,tabs+"End;");
}

void PascalExporter::segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs){
	insertar(prog, tabs+"Case "+expresion(getImpl<IT_SEGUN>(*(its[0])).expresion)+" Of");
	for (size_t i=1; i+1<its.size(); ++i) {
		if (its[i]->type==IT_DEOTROMODO) {
			insertar(prog, tabs+"\tElse Begin");
		} else {
			std::string e;
			for(auto &exp : getImpl<IT_OPCION>(*(its[i])).expresiones) {
				if (not e.empty()) e += ", ";
				e += expresion(exp);
			}
			insertar(prog, tabs+"\t"+e+": Begin");
		}
		bloque(prog, std::next(its[i]), its[i+1], tabs+"\t\t");
		insertar(prog, tabs+"\tEnd;");
	}
	insertar(prog, tabs+"End;");
}

void PascalExporter::repetir(t_output &prog, t_proceso_it it_repetir, t_proceso_it it_hasta, std::string tabs){
	insertar(prog, tabs+"Repeat");
	bloque(prog, std::next(it_repetir), it_hasta, tabs+"\t");
	auto &impl = getImpl<IT_HASTAQUE>(*it_hasta);
	if (impl.mientras_que)
		insertar(prog, tabs+"Until "+expresion(invert_expresion(impl.condicion))+";");
	else
		insertar(prog, tabs+"Until "+expresion(impl.condicion)+";");
}

void PascalExporter::para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) {
	auto &impl = getImpl<IT_PARA>(*it_para);
	std::string var = expresion(impl.contador), ini = expresion(impl.val_ini), 
		        fin = expresion(impl.val_fin), paso = impl.paso;
	if (paso=="1" or paso.empty()) {
		insertar(prog, tabs+"For "+var+":="+ini+" To "+fin+" Do Begin");
	} else if (paso=="-1") {
		insertar(prog, tabs+"For "+var+":="+ini+" DownTo "+fin+" Do Begin");
	} else {
		// no hay paso en pascal, emular con una auxiliar: 
		//      "para i desde 2 hasta 14 con paso 3 hacer ..." -> "para aux desde 0 hasta 4 hacer i=2+aux*3 ..."
		if (paso[0]=='-') {
			string aux=fin; fin=ini; ini=aux;
			paso=paso.substr(1);
		}
		string auxvar=get_aux_varname("aux_para_");
		memoria->DefinirTipo(auxvar,vt_numerica,true);
		insertar(prog,tabs+"For "+auxvar+":=0 To (("+fin+"-"+ini+") Div "+paso+") Do Begin");
		string aux_line=tabs+"\t"+var+" := "+ini+"+"+auxvar+"*"+colocarParentesis(paso)+";";
		if (!for_test) aux_line+=" // en Pascal el paso del ciclo For solo puede ser 1 o -1, por eso se usa este auxiliar para emular un paso diferente";
		insertar(prog,aux_line);
	}
	bloque(prog, std::next(it_para), it_fin, tabs+"\t");
	insertar(prog, tabs+"End;");
}

void PascalExporter::paracada(t_output &out, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) {
	auto &impl = getImpl<IT_PARACADA>(*it_para);
	std::string arreglo = ToLower(impl.arreglo), identif = ToLower(impl.identificador);
	const int *dims=memoria->LeerDims(arreglo);
	int n=dims[0];
	
	string *auxvars=new string[n];
	for(int i=0;i<n;i++) auxvars[i]=get_aux_varname("aux_para_");
	
	string vname=arreglo;
	for(int i=0;i<n;i++) { 
		string idx=auxvars[i];
		memoria->DefinirTipo(idx,vt_numerica,true);
		insertar(out,tabs+"For "+idx+":=Low("+vname+") To High("+vname+") Do Begin");
		if (vname==arreglo) vname+="["; else vname[vname.size()-1]=',';
		vname+=idx+"]";
		tabs+="\t";
	}
	
	for(int i=n-1;i>=0;i--) release_aux_varname(auxvars[i]);
	delete []auxvars;
	
	t_output aux_out;
	bloque(aux_out,std::next(it_para),it_fin,tabs);
	replace_var(aux_out,identif,vname);
	insertar_out(out,aux_out);
	
	for(int i=n-1;i>=0;i--) { 
		tabs=tabs.substr(0,tabs.size()-1);
		insertar(out,tabs+"End;");
	}
	memoria->RemoveVar(identif);
}



string PascalExporter::function(string name, string args) {
	if (name=="SEN") {
		return string("Sin")+args;
	} else if (name=="TAN") {
		uses_math=true;
		return string("Tan")+args;
	} else if (name=="ASEN") {
		uses_math=true;
		return string("Arcsin")+args;
	} else if (name=="ACOS") {
		uses_math=true;
		return string("Arccos")+args;
	} else if (name=="COS") {
		return string("Cos")+args;
	} else if (name=="RAIZ") {
		return string("Sqrt")+args;
	} else if (name=="RC") {
		return string("Sqrt")+args;
	} else if (name=="ABS") {
		return string("Abs")+args;
	} else if (name=="LN") {
		return string("Ln")+args;
	} else if (name=="EXP") {
		return string("Exp")+args;
	} else if (name=="AZAR") {
		uses_randomize=true;
		return string("Random")+args;
	} else if (name=="ATAN") {
		return string("Arctan")+args;
	} else if (name=="TRUNC") {
		return string("Trunc")+args;
	} else if (name=="REDON") {
		return string("Round")+args;
	} else if (name=="CONCATENAR") {
		return get_arg(args,1)+"+"+get_arg(args,2);
	} else if (name=="LONGITUD") {
		return string("Length")+args;
	} else if (name=="MAYUSCULAS") {
		return string("Upcase")+args;
	} else if (name=="MINUSCULAS") {
		return string("Lowercase")+args;
	} else if (name=="CONVERTIRANUMERO") {
		uses_sysutils=true;
		return string("StrToFloat")+args;
	} else if (name=="CONVERTIRATEXTO") {
		uses_sysutils=true;
		return string("FloatToStr")+args;
	} else if (name=="SUBCADENA") {
		string desde=get_arg(args,2);
		string cuantos=sumarOrestarUno(get_arg(args,3)+"-"+get_arg(args,2),true);
		if (input_base_zero_arrays) desde=sumarOrestarUno(desde,true);
		return string("Copy(")+get_arg(args,1)+","+desde+","+cuantos+")";
	} else {
		return ToLower(name)+args; // no deberia pasar esto
	}
}

// funcion usada por declarar_variables para las internas de una funcion
// y para obtener los tipos de los argumentos de la funcion para las cabeceras
string PascalExporter::get_tipo(map<string,tipo_var>::iterator &mit, bool for_func, bool by_ref) {
	tipo_var &t=mit->second;
	string stipo="SIN_TIPO",var=by_ref?"Var ":"",dims;
	if (t==vt_caracter) stipo="String";
	else if (t==vt_numerica) stipo=t.rounded?"Integer":"Real";
	else if (t==vt_logica) stipo="Boolean";
	else uses_sin_tipo=true;
	if (t.dims) {
		dims=make_dims(t.dims,"[","][","]",!for_func);
		if (replace_all(dims,"[0]","[ARREGLO_MAX]")) use_arreglo_max=true;
		replace_all(dims,"[",input_base_zero_arrays?"[0..":"[1..");
		replace_all(dims,"][",",");
		dims=string("Array ")+dims+" Of ";
	}
	return var+ToLower(mit->first)+": "+dims+stipo;
}

// resolucion de tipos (todo lo que acceda a cosas privadas de memoria tiene que estar en esta clase porque es la unica amiga)
void PascalExporter::declarar_variables(t_output &prog) {
	map<string,tipo_var>::iterator mit=memoria->GetVarInfo().begin(), mit2=memoria->GetVarInfo().end();
	if (mit!=mit2) prog.push_back("Var");
	string tab("\t");
	while (mit!=mit2) {
		prog.push_back(tab+get_tipo(mit)+";");
		++mit;
	}
}

// retorna el tipo y elimina de la memoria a esa variable
// se usa para armar las cabeceras de las funciones, las elimina para que no se
// vuelvan a declarar adentro
string PascalExporter::get_tipo(string name, bool by_ref, bool do_erase) {
	map<string,tipo_var>::iterator mit=memoria->GetVarInfo().find(name);
	if (mit==memoria->GetVarInfo().end()) 
		return ToLower(name)+": SIN_TIPO"; // puede pasar si hay variables que no se usan dentro de la funcion
	string ret = get_tipo(mit,true,by_ref);
	if (do_erase) memoria->GetVarInfo().erase(mit);
	return ret;
}

void PascalExporter::header(t_output &out) {
	if (uses_math||uses_crt||uses_sysutils) {
		insertar(out,"Uses");
		if (uses_crt) insertar(out,"\tCrt,");
		if (uses_math) insertar(out,"\tMath,");
		if (uses_sysutils) insertar(out,"\tSysutils,");
		string &last=out.back();
		last[last.size()-1]=';';
	}
	if (uses_sin_tipo) {
		insertar(out,"Type");
		if (!for_test) {
			insertar(out,"\t// Para las variables que no se pudo determinar el tipo se utiliza la constante");
			insertar(out,"\t// SIN_TIPO. El usuario debe reemplazar sus ocurrencias por el tipo adecuado.");
		}
		insertar(out,"\tSIN_TIPO = Real;");
	}
	if (use_arreglo_max) {
		insertar(out,"Const");
		if (!for_test) {
			insertar(out,"\t// En Pascal no se puede dimensionar un arreglo estático con una dimensión no constante.");
			insertar(out,"\t// Por esto se sobredimensionarán los arreglos que debieran ser dinámicos utilizando un");
			insertar(out,"\t// valor constante ARREGLO_MAX.");
		}
		insertar(out,"\tARREGLO_MAX = 100;");
	}
}

void PascalExporter::translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) {
	
	//cuerpo del proceso
	t_output out_proc;
	bloque(out_proc,++proc.begin(),proc.end(),"\t");
	
	// cabecera del proceso
	if (!f) {
		insertar(out,string("Program ")+ToLower(GetRT().funcs.GetMainName())+";");
		insertar(out,linea_special_para_reemplazar_por_uses_y_otros);
	} else {
		string dec,ret;
		if (f->nombres[0]=="") {
			dec="Procedure "; 
		} else {
			dec="Function ";
			ret=get_tipo(f->nombres[0],false,true);
			ret=ret.substr(ret.find(":"));
			replace_var(out_proc,ToLower(f->nombres[0]),ToLower(f->id));
		}
		dec+=ToLower(f->id);
		if (f->GetArgsCount()) {
			dec+="(";
			for(int i=1;i<=f->GetArgsCount();i++) {
				if (i!=1) dec+="; ";
				dec+=get_tipo(f->nombres[i],f->pasajes[i]==PP_REFERENCIA||memoria->LeerDims(f->nombres[i]),true);
			}
			dec+=")";
		}
		insertar(out,dec+ret+";");
	}
	
	declarar_variables(out);
	if (!f) insertar(out,linea_special_para_reemplazar_por_subprocesos);
	insertar(out,"Begin");
	if (uses_randomize) insertar(out,"\tRandomize;");
	
	insertar_out(out,out_proc);
	insertar(out,f?"End;":"End.");
	
	if (!for_test) insertar(out,"");
}

void PascalExporter::translate(t_output &out, Programa &prog) {
	load_subs_in_funcs_manager(prog);
	
	// cppcheck-suppress unusedScopedObject
	TiposExporter(prog,true); // para que se cargue el mapa_memorias con memorias que tengan ya definidos los tipos de variables que correspondan
	
	init_header(out,"// ");
	
	t_output procedures;
	translate_all_procs(out,procedures,prog);
	
	t_output_it it1=find(out.begin(),out.end(),string(linea_special_para_reemplazar_por_subprocesos));
	it1=out.erase(it1); 
	if (!procedures.empty()) {
		if (!for_test) it1=++out.insert(it1,"");
		if (!for_test && has_matrix_func) {
			it1=++out.insert(it1,"// Para poder pasar un arreglo como parametro a un procedimiento o a una funcion");
			it1=++out.insert(it1,"// en Pascal hay que declarar un tipo para el mismo en la sección Type del");
			it1=++out.insert(it1,"// programa, y utilizar ese tipo para la declaración del procedimiento o la");
			it1=++out.insert(it1,"// función. La traducción automática de PSeInt no hace esto, por lo que el");
			it1=++out.insert(it1,"// código generado no es completamente correcto.");
			it1=++out.insert(it1,"");
		}
		for(t_output_it it2=procedures.begin();it2!=procedures.end();++it2) it1=++out.insert(it1,*it2);
	}
	
	t_output uses_const_type;
	header(uses_const_type);
	
	/*t_output_it */it1=find(out.begin(),out.end(),string(linea_special_para_reemplazar_por_uses_y_otros));
	it1=out.erase(it1); if (!for_test) it1=++out.insert(it1,"");
	for(t_output_it it2=uses_const_type.begin();it2!=uses_const_type.end();++it2) it1=++out.insert(it1,*it2);	
	
}

string PascalExporter::get_constante(string name) {
	if (name=="PI") return "PI";
	if (name=="VERDADERO") return "True";
	if (name=="FALSO") return "False";
	return name;
}

string PascalExporter::get_operator(string op, bool for_string) {
	// para agrupar operaciones y alterar la jerarquia
	if (op=="(") return "("; 
	if (op==")") return ")";
	// para llamadas a funciones
	if (op=="{") return "("; 
	if (op=="}") return ")";
	// para indices de arreglos
	if (op=="[") return "[";
	if (op==",") return ",";
	if (op=="]") return "]";
	// algebraicos, logicos, relaciones
	if (op=="+") return "+"; 
	if (op=="-") return "-"; 
	if (op=="/") return "/"; 
	if (op=="*") return "*";
	if (op=="^") return "func Power(arg1,arg2)";
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

string PascalExporter::make_string (string cont) {
	for(unsigned int i=0;i<cont.size();i++)
		if (cont[i]=='\\') cont.insert(i++,"\\");
	return string("\'")+cont+"\'";
}

void PascalExporter::comentar (t_output & prog, string text, std::string tabs) {
	if (!for_test) insertar(prog,tabs+"// "+text);
}
