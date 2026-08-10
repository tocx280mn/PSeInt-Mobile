#include <sstream>
#include <cstdlib>
#include "version.h"
#include "exportexp.h"
#include "ExporterBase.hpp"
#include "CExporter.hpp"
using namespace std;

CExporter::CExporter():CppExporter() {
	use_get_aux_buffer=false;
	read_strings=false;
	use_bool=false;
	declare_cstrings=false;
	use_func_subcadena=false;
	use_reference=false;
}

void CExporter::borrar_pantalla(t_output &prog, std::string tabs) {
	if (for_test)
		insertar(prog,tabs+"printf(\"\\n\");");
	else
		insertar(prog,tabs+"printf(\"\\n\"); /* no hay forma directa de borrar la pantalla con C estandar */");
}

void CExporter::esperar_tecla(t_output &prog, std::string tabs){
	if (for_test)
		insertar(prog,tabs+"getchar();");
	else
		insertar(prog,tabs+"getchar(); /* a diferencia del pseudocódigo, espera un Enter, no cualquier tecla */");
}

void CExporter::escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs){
	string format,arglist;
	t_arglist_it it=args.begin();
	while (it!=args.end()) {
		tipo_var t;
		string varname=expresion(*it,t);
		if (es_cadena_constante(*it)) {
			format+=varname.substr(1,varname.size()-2);
		} else {
			if (t==vt_numerica && t.rounded) {
				format+="%i"; arglist+=", "; arglist+=varname;
			} else if (t==vt_numerica) {
				format+="%f"; arglist+=", "; arglist+=varname;
			} else if (t==vt_logica) {
				format+="%i"; arglist+=", "; arglist+=varname;
			} else if (t==vt_caracter) {
				format+="%s"; arglist+=", "; arglist+=varname;
			} else {
				format+="%f"; arglist+=", "; arglist+=varname;
			}
		}
		++it;
	}
	if (saltar) format+="\\n";
	insertar(prog,tabs+"printf(\""+format+"\""+arglist+");");
}

void CExporter::leer(t_output &prog, t_arglist args, std::string tabs) {
	t_arglist_it it=args.begin();
	while (it!=args.end()) {
		tipo_var t;
		string varname=expresion(*it,t);
		if (t==vt_numerica && t.rounded) insertar(prog,tabs+"scanf(\"%i\", &"+varname+");");
		else if (t==vt_numerica) insertar(prog,tabs+"scanf(\"%f\", &"+varname+");");
		else if (t==vt_logica) insertar(prog,tabs+"scanf(\"%i\", &"+varname+");");
		else if (t==vt_caracter) { read_strings=true; insertar(prog,tabs+"scanf(\"%s\", "+varname+");"); }
		else insertar(prog,tabs+"scanf(\"%f\", "+varname+");");
		++it;
	}
}


void CExporter::para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) {
	auto &impl = getImpl<IT_PARA>(*it_para);
	string var = expresion(impl.contador), ini = expresion(impl.val_ini), 
		   fin = expresion(impl.val_fin), paso = impl.paso;
	if ((not paso.empty()) and paso[0]=='-') {
		if (paso=="-1")
			insertar(prog,tabs+"for ("+var+"="+ini+"; "+var+">="+fin+"; --"+var+") {");
		else
			insertar(prog,tabs+"for ("+var+"="+ini+"; "+var+">="+fin+"; "+var+"-="+expresion(paso.substr(1,paso.size()-1))+") {");
	} else {
		if (paso=="1" or paso.empty())
			insertar(prog,tabs+"for ("+var+"="+ini+"; "+var+"<="+fin+"; ++"+var+") {");
		else
			insertar(prog,tabs+"for ("+var+"="+ini+"; "+var+"<="+fin+"; "+var+"+="+expresion(paso)+") {");
	}
	bloque(prog,std::next(it_para),it_fin,tabs+"\t");
	insertar(prog,tabs+"}");
}

void CExporter::paracada(t_output &out, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) {
	auto &impl = getImpl<IT_PARACADA>(*it_para);
	string identif = ToLower(impl.identificador), arreglo = ToLower(impl.arreglo);
	string first = arreglo, last = arreglo;
	const int *dims=memoria->LeerDims(arreglo);
	if (!dims) { insertar(out,string("Error: ")+arreglo+" no es un arreglo"); return; }
	for (int i=1;i<=dims[0];++i) {
		first+="[0]";
		last+="[";
		last+=IntToStr(dims[i]-1);
		last+="]";
	}
	insertar(out,tabs+"for (typeof(&("+first+")) "+identif+"=&("+first+"); "+identif+"<=&("+last+"); ++"+identif+") {");
	t_output aux_out;
	bloque(aux_out,std::next(it_para),it_fin,tabs+"\t");
	replace_var(aux_out,identif,string("(*"+identif+")"));
	memoria->RemoveVar(identif);
	insertar_out(out,aux_out);
	insertar(out,tabs+"}");
}



string CExporter::function(string name, string args) {
	if (name=="CONCATENAR") {
		return string("strcat(strcpy(get_aux_buffer(), ")+get_arg(args,1)+"), "+get_arg(args,2)+")";
	} else if (name=="LONGITUD") {
		return string("strlen("+get_arg(args,1))+")";
	} else if (name=="SUBCADENA") {
		use_func_subcadena=true;
		string desde=get_arg(args,2);
		string cuantos=sumarOrestarUno(get_arg(args,3)+"-"+get_arg(args,2),true);
		if (!input_base_zero_arrays) desde=sumarOrestarUno(desde,false);
		return string("subcadena(")+get_arg(args,1)+", "+desde+", "+cuantos+")";
	} else if (name=="CONVERTIRANUMERO") {
		include_cstdlib=true;
		string s=get_arg(args,1);
		return string("atof(")+s+")";
	} else {
		if (name=="MAYUSCULAS"||name=="MINUSCULAS"||name=="CONVERTIRATEXTO") use_get_aux_buffer=true;
		return CppExporter::function(name,args); // las demás son iguales que en la conversión a C++
	}
}

// funcion usada por declarar_variables para las internas de una funcion
// y para obtener los tipos de los argumentos de la funcion para las cabeceras
string CExporter::get_tipo(map<string,tipo_var>::iterator &mit, bool for_func, bool by_ref) {
	tipo_var &t=mit->second;
	string stipo="SIN_TIPO ", spost;
	if (t==vt_caracter) { stipo="char "; if (for_func) stipo+="*"; else { declare_cstrings=true; spost="[MAX_STRLEN]"; } }
	else if (t==vt_numerica) stipo=t.rounded?"int ":"float ";
	else if (t==vt_logica) { use_bool=true; stipo="bool "; }
	else use_sin_tipo=true;
	if (t.dims) {
		string ret=stipo+ToLower(mit->first)+spost+make_dims(t.dims,"[","][","]",!for_func);
		while (ret.find("[0]")!=string::npos) {
			use_arreglo_max=true;
			ret.replace(ret.find("[0]"),3,"[ARREGLO_MAX]");
		}
		return ret;
	} else {
		if (by_ref) stipo+="*";
		return stipo+ToLower(mit->first)+spost;
	}
}


void CExporter::header(t_output &out) {
	if (use_func_mayusculas||use_func_minusculas||use_func_subcadena) use_get_aux_buffer=true;
	if (use_func_subcadena) use_string=true;
	// cabecera
	init_header(out,"/* "," */");
	out.push_back("#include<stdio.h>");
	if (include_cmath) out.push_back("#include<math.h>");
	if (include_cstdlib) out.push_back("#include<stdlib.h>");
	if (use_func_esperar) out.push_back("#include<time.h>");
	if (use_bool) out.push_back("#include<stdbool.h>");
	if (use_func_mayusculas||use_func_minusculas) out.push_back("#include<ctype.h>");
	if (use_string) out.push_back("#include<string.h>");
	if (!for_test) out.push_back("");
	if (use_func_esperar) {
		if (!for_test) out.push_back("/* No hay en el C estandar una funcion equivalente a \"esperar\", pero puede programarse. */");
		out.push_back("void esperar(double t);");
		if (!for_test) out.push_back("");
	}
	if (use_func_convertiratexto) {
		if (!for_test) out.push_back("/* No hay en el C estandar una funcion equivalente a \"convertiratexto\", pero puede programarse. */");
		out.push_back("char *convertiratexto(float f);");
		if (!for_test) out.push_back("");
	}
	if (use_func_mayusculas) {
		if (!for_test) out.push_back("/* No hay en el C estandar una funcion equivalente a \"mayusculas\", pero puede programarse. */");
		out.push_back("char *mayusculas(const char *s);");
		if (!for_test) out.push_back("");
	}
	if (use_func_minusculas) {
		if (!for_test) out.push_back("/* No hay en el C estandar una funcion equivalente a \"minusculas\", pero puede programarse. */");
		out.push_back("char *minusculas(const char *s);");
		if (!for_test) out.push_back("");
	}
	if (use_func_subcadena) {
		if (!for_test) out.push_back("/* No hay en el C estandar una funcion equivalente a \"subcadena\", pero puede programarse. */");
		out.push_back("char *subcadena(const char *s, int desde, int cuantos);");
		if (!for_test) out.push_back("");
	}
	if (use_get_aux_buffer) {
		if (!for_test) out.push_back("/* Función auxiliar para el manejo de cstrings temporales, ver detalles más abajo. */");
		out.push_back("char *get_aux_buffer(double t);");
	}
	if (use_arreglo_max) {
		if (!for_test) {
			out.push_back("/* En C no se puede dimensionar un arreglo estático con una dimensión no constante.");
			out.push_back("   PSeInt sobredimensionará el arreglo utilizando un valor simbólico ARREGLO_MAX.");
			out.push_back("   Sería posible crear un arreglo dinámicamente con los operadores new y delete, pero");
			out.push_back("   este mecanismo aún no está soportado en las traducciones automáticas de PSeInt. */");
		}
		out.push_back("#define ARREGLO_MAX 100");
		if (!for_test) out.push_back("");
	}
	if (declare_cstrings) {
		if (!for_test) {
			out.push_back("/* En C no hay variables para guardar cadenas de texto, sino que debe construirse");
			out.push_back("   un arreglo de caracteres (tipo char). El tamaño del arreglo determina la longitud");
			out.push_back("   máxima que puede tener la cadena que guarda (tamaño-1, por el caracter de terminación).");
			out.push_back("   La constante MAX_STRLEN define el tamaño máximo que se utiliza en este programa para");
			out.push_back("   cualquier cadena. */");
			
		}
		out.push_back("#define MAX_STRLEN 256");
		if (!for_test) out.push_back("");
	}
	if (use_sin_tipo) {
		if (!for_test) {
			out.push_back("/* Para las variables que no se pudo determinar el tipo se utiliza la constante");
			out.push_back("   SIN_TIPO. El usuario debe reemplazar sus ocurrencias por el tipo adecuado");
			out.push_back("   (usualmente int,float,bool, o char[]). */");
		}
		out.push_back("#define SIN_TIPO float");
		if (!for_test) out.push_back("");
	}
	if (read_strings) {
		if (!for_test) {
			out.push_back("/* Para leer variables de texto se utiliza scanf, que lee solo una palabra. ");
			out.push_back("   Para leer una linea completa (es decir, incluyendo los espacios en blanco)");
			out.push_back("   se debe utilzar ges (ej, reemplazar scanf(\"%s\",x) por gets(x)) pero ");
			out.push_back("   obliga a agregar un getchar() antes del gets si antes del mismo se leyó");
			out.push_back("   otra variable con scanf. */");
			out.push_back("");
		}
	}
	if (use_reference) {
		if (!for_test) {
			out.push_back("/* El algoritmo contiene funciones que reciben argumentos por referencia.");
			out.push_back("   Cuando estos argumentos no se corresponden con arreglos, al llamar a la");
			out.push_back("   función el argumento actual debe ser un puntero, lo cual se logra ");
			out.push_back("   anteponiendo el operador & (a la variable que se pasa como argumento");
			out.push_back("   actual). En estos casos, la traducción automática falla, por lo que");
			out.push_back("   debe agregarlo manualmente donde sea necesario. */");
			out.push_back("");
		}
	}
}

void CExporter::footer(t_output &out) {
	if (use_get_aux_buffer) {
		if (!for_test) { 
			out.push_back("");
			out.push_back("/* Algunas operaciones de cadenas de texto requieren cadenas auxiliares temporales. ");
			out.push_back("   Por ejemplo, la concatenación de cadenas. Como las cadenas son en realidad arreglos"); 
			out.push_back("   no hay forma simple de crear estos temporales dinámicamente y destruirlos cuando ya"); 
			out.push_back("   no sean necesarios. Si cada función que requiere un buffer auxiliar usa su propio"); 
			out.push_back("   buffer estático, entonces no se puede invocar a esa función dos veces en la misma"); 
			out.push_back("   expresión (porque ambas invocaciones retornarían un mismo buffer como resultado"); 
			out.push_back("   aunque sus argumentos sean diferentes, y además esto genera un comportamiento ");
			out.push_back("   indefinido según el estándar de C). Para salvar este problema, esta función define");
			out.push_back("   varios buffers auxiliares y los va entregando a las demás funciones a medida que");
			out.push_back("   estas los requieren, rotando cual entrega en orden. Entonces, una expresión puede");
			out.push_back("   requerir tantos buffers como MAX_BUFFERS indique sin que se generen problemas,");
			out.push_back("   y se evita utilizar memoria dinámica. */");
		}
		out.push_back("#define MAX_BUFFERS 10");
		out.push_back("char *get_aux_buffer(double t) {");
		out.push_back("\tstatic char buffers[MAX_BUFFERS][MAX_STRLEN];");
		out.push_back("\tstatic int count = -1;");
		out.push_back("\tcount = count+1;");
		out.push_back("\tif (count==MAX_BUFFERS) count = 0;");
		out.push_back("\treturn buffers[count];");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
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
		out.push_back("char *convertiratexto(float f) {");
		out.push_back("\tchar *buf = get_aux_buffer();");
		out.push_back("\tsprintf(buf, \"%f\", f);");
		out.push_back("\treturn buf;");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
	if (use_func_mayusculas) {
		if (!for_test) out.push_back("");
		out.push_back("char *mayusculas(const char *s) {");
		out.push_back("\tchar *buf = get_aux_buffer();");
		out.push_back("\tfor (unsigned int i=0; i<s.size(); ++i)");
		out.push_back("\t\tbuf[i] = toupper(s[i]);");
		out.push_back("\treturn buf;");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
	if (use_func_minusculas) {
		if (!for_test) out.push_back("");
		out.push_back("char *minusculas(const char *s) {");
		out.push_back("\tchar *buf = get_aux_buffer();");
		out.push_back("\tfor (unsigned int i=0; i<s.size(); ++i)");
		out.push_back("\t\tbuf[i] = tolower(s[i]);");
		out.push_back("\treturn buf;");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
	if (use_func_subcadena) {
		if (!for_test) out.push_back("");
		out.push_back("char *subcadena(const char *s, int desde, int cuantos) {");
		out.push_back("\tchar *buf = get_aux_buffer();");
		out.push_back("\tstrncpy(buf, s+desde,cuantos);");
		out.push_back("\tbuf[cuantos] = \'\\0\';");
		out.push_back("\treturn buf;");
		out.push_back("}");
		if (!for_test) out.push_back("");
	}
}


void CExporter::translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) {

	//cuerpo del proceso
	t_output out_proc;
	bloque(out_proc,++proc.begin(),proc.end(),"\t");
	
	// cabecera del proceso
	string ret; // sentencia "return ..." de la funcion
	if (!f) {
		out.push_back("int main() {");
		ret="return 0";
	} else {
		string dec;
		if (f->nombres[0]=="") {
			dec="void "; 
		} else {
			ret=CppExporter::get_tipo(f->nombres[0],false,false);
			dec=ret.substr(0,ret.find(" ")+1);
			ret=string("return")+ret.substr(ret.find(" "));
		}
		dec+=ToLower(f->id)+"(";
		for(int i=1;i<=f->GetArgsCount();++i) {
			if (i!=1) dec+=", ";
			string var_dec=CppExporter::get_tipo(f->nombres[i],f->pasajes[i]==PP_REFERENCIA);
			dec+=var_dec;
			if (f->pasajes[i]==PP_REFERENCIA && var_dec.find("[")==string::npos) {
				use_reference = true;
				replace_var(out_proc,ToLower(f->nombres[i]),ToLower(string("(*")+f->nombres[i]+(")")));
			}
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


string CExporter::get_operator(string op, bool for_string) {
	if (for_string) {
		if (for_string) {
			use_string=true;
			if (op=="+") { use_get_aux_buffer=true; return "func strcat(strcpy(get_aux_buffer(), arg1), arg2)"; }
			if (op=="=") return "func strcmp(arg1, arg2)==0"; 
			if (op=="<>") return "func strcmp(arg1, arg2)!=0"; 
			if (op=="<") return "func strcmp(arg1, arg2)<0"; 
			if (op==">") return "func strcmp(arg1, arg2)>0"; 
			if (op=="<=") return "func strcmp(arg1, arg2)<=0"; 
			if (op==">=") return "func strcmp(arg1, arg2)>=0"; 
		}
	} 
	return CppExporter::get_operator(op,false);
}

void CExporter::comentar (t_output & prog, string text, std::string tabs) {
	if (!for_test) insertar(prog,tabs+"/* "+text+" */");
}

std::string CExporter::referencia (const std::string &exp) {
	return "&"+colocarParentesis(exp);
}

