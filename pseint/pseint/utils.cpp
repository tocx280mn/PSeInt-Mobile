#include "global.h"
#include "common.h"
#include "Evaluar.hpp"
#include "intercambio.h"
#include "new_memoria.h"
#include "utils.h"
#include "zcurlib.h"
#include "FuncsManager.hpp"
#include "RunTime.hpp"
#include "strFuncs.hpp"
#include <cmath>

void show_user_info(std::string msg) {
	if (fix_win_charset) fixwincharset(msg);
	if (colored_output) setForeColor(COLOR_INFO);
	if (with_io_references) Inter.SendErrorPositionToTerminal(); // para que no asocie el error/mensaje con la última entrada/salida
	std::cout << msg << std::endl;
}

void show_user_info(std::string msg1, int num, std::string msg2) {
	if (fix_win_charset) { fixwincharset(msg1); fixwincharset(msg2); }
	if (colored_output) setForeColor(COLOR_INFO);
	if (with_io_references) Inter.SendErrorPositionToTerminal(); // para que no asocie el error/mensaje con la última entrada/salida
	std::cout << msg1 << num << msg2 << std::endl;
}


// ***************** Control de Errores y Depuración **********************

void WarnError_impl(int num, std::string s, bool runtime) {
	if (runtime!=Inter.IsRunning()) return;
	if (Inter.EvaluatingForDebug()) {
		Inter.SetError(std::string("<<")+s+">>");
	} else {
		if (raw_errors) {
			std::cout << "=== Line " << Inter.GetLocation().linea << ": Warning " << num << std::endl;
		} else {
			if (colored_output) setForeColor(COLOR_WARNING);
			if (with_io_references) Inter.SendErrorPositionToTerminal();
			if (runtime)
				std::cout << "*** Advertencia " << num << ": " << s << std::endl;
			else
				std::cout << "Lin " << Inter.GetLocation().linea << " (inst " << Inter.GetLocation().instruccion << "): Advertencia " << num << ": " << s << std::endl;
		}
	}
}

// ------------------------------------------------------------
//    Informa un error en tiempo de ejecucion
// ------------------------------------------------------------
void ExeError_impl(int num, std::string s) { 
	if (Inter.EvaluatingForDebug()) {
		Inter.SetError(std::string("<<")+s+">>");
	} else {
		if (raw_errors) {
			std::cout << "=== Line " << Inter.GetLocation().linea << ": ExeError " << num << std::endl;
			exit(0);
		}
		if (colored_output) setForeColor(COLOR_ERROR);
		if (with_io_references) Inter.SendErrorPositionToTerminal();
		std::cout << "Lin " << Inter.GetLocation().linea << " (inst " << Inter.GetLocation().instruccion << "): ERROR " << num << ": " << s << std::endl;
		for(int i=Inter.GetBacktraceLevel()-1;i>0;i--) {  
			FrameInfo fi=Inter.GetFrame(i);
			std::cout << "...dentro del subproceso " << fi.func_name << ", invocado desde la línea " << fi.loc.linea << "." << std::endl;
		}
		if (ExeInfoOn) {
			ExeInfo << "Lin " << Inter.GetLocation().linea << " (inst " << Inter.GetLocation().instruccion << "): ERROR " << num << ": " << s << std::endl;
			for(int i=Inter.GetBacktraceLevel()-1;i>0;i--) {  
				FrameInfo fi=Inter.GetFrame(i);
				ExeInfo << "Lin " << fi.loc.linea<<" (inst " << fi.loc.instruccion<<"): ";
				ExeInfo << "...dentro del subproceso " << fi.func_name << ", invocado desde aquí." << std::endl;
			}
			ExeInfo << "*** Ejecucion Interrumpida. ***" << std::endl;
		} 
		if (wait_key) {
			show_user_info("*** Ejecución Interrumpida. ***");
		}
//		Inter.AddError(s,Inter.GetLocation().linea);
		if (ExeInfoOn) ExeInfo.close();
		if (wait_key) getKey();
		exit(0);
	}
}

// ------------------------------------------------------------
//    Informa un error de syntaxis antes de la ejecucion
// ------------------------------------------------------------
void SynError_impl(int num, std::string s) { 
	SynError_impl(num,s,Inter.GetLocation());
}

void SynError_impl(int num, std::string s, CodeLocation loc) { 
#ifdef _FOR_PSEXPORT
	return;
#endif
	if (raw_errors) {
		std::cout << "=== Line " << loc.linea << ": SynError " << num << std::endl;
		return;
	}
	if (Inter.EvaluatingForDebug()) {
		Inter.SetError(std::string("<<")+s+">>");
	} else {
		if (colored_output) setForeColor(COLOR_ERROR);
		if (with_io_references) Inter.SendErrorPositionToTerminal(); // para que no asocie el error con la última entrada/salida
		std::cout << "Lin " << loc.linea;
		if (loc.instruccion>0) std::cout << " (inst " << loc.instruccion << ")";
		std::cout << ": ERROR " << num << ": " << s << std::endl;
		if (ExeInfoOn) {
			ExeInfo << "Lin " << loc.linea;
			if (loc.instruccion>0) ExeInfo << " (inst " << loc.instruccion << ")";
			ExeInfo << ": ERROR " << num << ": " << s << std::endl;
		}
	}
}


// ------------------------------------------------------------
//    Comprueba que sea un identificador correcto y unico
//    A diferencia del anterior, no tiene en cuenta las
//  funciones predefinidas.
// ------------------------------------------------------------
bool CheckVariable(RunTime &rt, std::string str, int errcode) { 
	size_t pi=str.find("(",0);
	if (pi!=std::string::npos && str[str.size()-1]==')') {
		CheckDims(rt,str);
		str.erase(pi,str.size()-pi); // si es arreglo corta los subindices
	}
	bool ret=true;
	for (size_t x=0;x<str.size();x++) {
		if (!EsLetra(str[x],x!=0,lang[LS_ALLOW_ACCENTS]))
			ret=false;
	}
	// Comprobar que no sea palabra reservada
	if (rt.funcs.IsFunction(str)) 
		ret=false;
	else if (str=="LEER" || str=="ESCRIBIR" || str=="MIENTRAS" || str=="HACER" || str=="SEGUN" || str=="VERDADERO" || str=="FALSO" || str=="PARA")
		ret=false;
	else if (str=="REPETIR" || str=="SI" || str=="SINO" || str=="ENTONCES" || str=="DIMENSION" || str=="PROCESO" || str=="FINSI" ||  str=="" || str=="FINPARA")
		ret=false;
	else if (str=="FINSEGUN" || str=="FINPROCESO" || str=="FINMIENTRAS" || str=="HASTA" || str=="DEFINIR" || str=="COMO")
		ret=false;
	else if (lang[LS_ENABLE_USER_FUNCTIONS] && (str=="FINSUBPROCESO" || str=="SUBPROCESO" ||str=="FINFUNCION" || str=="FUNCION" ||str=="FINFUNCIÓN" || str=="FUNCIÓN") )
		ret=false;
	if (!ret && errcode!=-1) rt.err.SyntaxError(errcode,std::string("Identificador no válido (")+str+")."); 
	return ret;
}

// ----------------------------------------------------------------------
//    Compara los comienzos de dos cadenas (case sensitive)
// ----------------------------------------------------------------------
//bool LeftCompare(string a, string b) {  
//	// compara los caracteres de la izquierda de a con b
//	bool ret;
//	if (a.size()<b.size()) ret=false; else {
//		a.erase(b.size(),a.size()-b.size());
//		if (a==b) ret=true; else ret=false; }
//	return ret; }

// ----------------------------------------------------------------------
//    Compara los comienzos de dos cadenas (case insensitve)
// ----------------------------------------------------------------------
//bool LeftCompareNC(string a, string b){  
//	// compara los caracteres de la izquierda de a con b
//	for (int x=0;x<(int)a.size();x++)
//		if (a[x]>96 && a[x]<123) a[x]-=32;
//	for (int x=0;x<(int)b.size();x++)
//		if (b[x]>96 && b[x]<123) b[x]-=32;
//	bool ret;
//	if (a.size()<b.size()) ret=false; else {
//		a.erase(b.size(),a.size()-b.size());
//		if (a==b) ret=true; else ret=false; }
//	return ret; 
//}


// ----------------------------------------------------------------------
//    Compara las terminaciones de dos cadenas (case insensitve)
// ----------------------------------------------------------------------
//bool RightCompareNC(string a, string b) { 
//	// compara los caracteres de la derecha de a con b
//	for (int x=0;x<(int)a.size();x++)
//		if (a[x]>96 && a[x]<123) a[x]-=32;
//	for (int x=0;x<(int)b.size();x++)
//		if (b[x]>96 && b[x]<123) b[x]-=32;
//	bool ret;
//	if (a.size()<b.size()) ret=false; else {
//		a.erase(0,a.size()-b.size());
//		if (a==b) ret=true; else ret=false; }
//	return ret; 
//}

// ----------------------------------------------------------------------
//    Compara parte de una cadena con otra (case insensitve)
// ----------------------------------------------------------------------
//bool MidCompareNC(string a, string b, int from) { 
//	unsigned int to=from+a.size();
//	// controla los tamaños y corta la parte de interes
//	if (b.size()<to) return false;
//	b.erase(to,b.size());
//	b.erase(0,from);
//	for (int x=0;x<(int)a.size();x++)
//		if (a[x]>96 && a[x]<123) a[x]-=32;
//	for (int x=0;x<(int)b.size();x++)
//		if (b[x]>96 && b[x]<123) b[x]-=32;
//	bool ret;
//	// compara los caracteres de la derecha de a con b
//	if (a.size()<b.size()) ret=false; else {
//		a.erase(0,a.size()-b.size());
//		if (a==b) ret=true; else ret=false; }
//	return ret; 
//}

// ----------------------------------------------------------------------
//    Averigua el tipo de variable para un dato
// ----------------------------------------------------------------------
tipo_var GuestTipo(std::string str) { 
	tipo_var ret=vt_desconocido;
	std::string strb;
	strb=str;
	if (str.size()==0) ret=vt_desconocido; else {
		if (strb=="VERDADERO" || strb=="FALSO") {
			ret=vt_caracter_o_logica;
		} else {
			ret=vt_caracter_o_numerica;
			bool punto=false;
			for (int x=0;x<(int)str.size();x++) {
				if (str[x]=='.') {
					if (punto) { ret=vt_caracter; break; }
					else punto=true;
				} else if (str[x]<'0' || str[x]>'9') {
					if (x||(str[x]!='-'&&str[x]!='+')) {
						ret=vt_caracter;
						break;
					}
				}
			}
		}
	}
	return ret;
}

// ----------------------------------------------------------------------
//    Reemplaza una cadena por otra si es que se encuentra
// ----------------------------------------------------------------------
bool ReplaceIfFound(std::string &str, std::string str1, std::string str2, bool saltear_literales) { 
	size_t x=str.find(str1,0);
	size_t ox=0;
	bool ret=false;
	while (x!=std::string::npos) {
		if (saltear_literales) {
			size_t xc=str.find("\'",ox);
			if (xc!=std::string::npos && xc<x) {
				xc=str.find("\'",xc+1);
				if (xc==std::string::npos) break;
				x=str.find(str1,ox=(xc+1));
				continue;
			}
		}
		ret=true;
		str.erase(x,str1.size());
		str.insert(x,str2);
		x=str.find(str1,ox=(x+str2.size()));
	}
	return ret; 
}


// **************************************************************************

// funciones auxiliares para psexport
inline int max(int a,int b){ return (a>b)?a:b; }
inline int min(int a,int b){ return (a<b)?a:b; }
std::string CutString(std::string s, int a, int b){
	std::string r=s;
	r.erase(0,a);
	if (b!=0) r.erase(r.size()-b,b);
	return r;
}

// determina si una letra puede ser parte de una palabra clave o identificador
bool parteDePalabra(char c) {
	return (EsLetra(c) || c=='_' || (c>='0' && c<='9'));
}

void fixwincharset(std::string &s, bool reverse) {
	if (!fix_win_charset) return;
	if (reverse) {
		for(unsigned int i=0;i<s.size();i++) { 
			char &c=s[i];
			if (c==-96) c='á';
			else if (c==-126) c='é';
			else if (c==-95) c='í';
			else if (c==-94) c='ó';
			else if (c==-93) c='ú';
			else if (c==-75) c='Á';
			else if (c==-112) c='É';
			else if (c==-42) c='Í';
			else if (c==-32) c='Ó';
			else if (c==-23) c='Ú';
			else if (c==-92) c='ñ';
			else if (c==-91) c='Ñ';
			else if (c==-83) c='¡';
			else if (c==-88) c='¿';
			else if (c==-127) c='ü';
			else if (c==-102) c='Ü';
		}
	} else {
		for(unsigned int i=0;i<s.size();i++) { 
			char &c=s[i];
			if (c=='á') c=-96;
			else if (c=='é') c=-126;
			else if (c=='í') c=-95;
			else if (c=='ó') c=-94;
			else if (c=='ú') c=-93;
			else if (c=='Á') c=-75;
			else if (c=='É') c=-112;
			else if (c=='Í') c=-42;
			else if (c=='Ó') c=-32;
			else if (c=='Ú') c=-23;
			else if (c=='ñ') c=-92;
			else if (c=='Ñ') c=-91;
			else if (c=='¡') c=-83;
			else if (c=='¿') c=-88;
			else if (c=='ü') c=-127;
			else if (c=='Ü') c=-102;
		}
	}
}

std::string NextToken(const std::string &cadena, int &p) {
	int l=cadena.size();
	while (p<l && (cadena[p]==' ' || cadena[p]=='\t')) p++;
	if (p==l) return "";
	int p1=p;
	if (EsLetra(cadena[p])) {
		while (p<l && parteDePalabra(cadena[p]) ) p++;
	} else if ((cadena[p]>='0'&&cadena[p]<='9')||cadena[p]=='.') {
		while (p<l && ((cadena[p]>='0'&&cadena[p]<='9')||cadena[p]=='.')) p++;
	} else if (cadena[p]=='\"'||cadena[p]=='\'') {
		while (p<l && cadena[p]!='\"'&&cadena[p]!='\'' ) p++;
	} else if (cadena[p]==')') {
		p++; return ")";
	} else if (cadena[p]=='(') {
		p++; return "(";
	} else { // operador? todavia no se usa este caso
		if (p<l) p++;
		while (p<l && !parteDePalabra(cadena[p]) && cadena[p]!=' ' && cadena[p]!='\t' && cadena[p]!=')' && cadena[p]!='(' && cadena[p]!=',' && cadena[p]!=';' && cadena[p]!='\"' && cadena[p]!='\'') p++;
	}
	return cadena.substr(p1,p-p1);
}

// arma el objeto Funcion analizando la cabecera (cadena, que viene sin la palbra PROCESO/SUBPROCESO)
// pero no lo registra en el mapa de subprocesos

std::unique_ptr<Funcion> MakeFuncionForSubproceso(RunTime &rt, const std::string &cadena, bool es_proceso) {
	return MakeFuncionForSubproceso(rt,SepararCabeceraDeSubProceso(cadena),es_proceso);
}

std::unique_ptr<Funcion> MakeFuncionForSubproceso(RunTime &rt, const FuncStrings &parts, bool es_proceso) {
	ErrorHandler &err_handler = rt.err;
	auto kw2str = [&keys=lang.keywords](int i) { return keys[i].get(); };
	
	auto the_func = std::make_unique<Funcion>(0); 
	
	// parsear nombre y valor de retorno
	if (parts.ret_id.empty()) {
		the_func->tipos[0]=vt_error; // para que cuando la quieran usar en una expresión salte un error, porque evaluar no verifica si se devuelve algo porque se use desde Ejecutar parala instrucción INVOCAR
	} else {
		if (es_proceso) err_handler.SyntaxError(242,"El proceso principal no puede retornar ningun valor.");
		the_func->nombres[0] = parts.ret_id;
		CheckVariable(rt,the_func->nombres[0]);
	}
	the_func->id = parts.nombre;
	if (parts.nombre.empty() or LeftCompare(parts.nombre,"<sin_titulo_")) { 
		err_handler.SyntaxError(40,MkErrorMsg("Falta nombre de $.",kw2str(es_proceso?KW_ALGORITMO:KW_SUBALGORITMO))); 
//		static int untitled_functions_count=0; // para numerar las funciones sin nombre
//		the_func->id = string("<sin_nombre>")+IntToStr(++untitled_functions_count);
	} else {
		if (rt.funcs.IsFunction(parts.nombre))
			err_handler.SyntaxError(243,MkErrorMsg("Ya existe otro $ con el mismo nombre($).",kw2str(KW_ALGORITMO)+" o "+kw2str(KW_SUBALGORITMO),parts.nombre+")."));
		else if (not CheckVariable(rt,parts.nombre))
			err_handler.SyntaxError(244,MkErrorMsg("El nombre de $ ($) no es un identificador válido.",kw2str(es_proceso?KW_ALGORITMO:KW_SUBALGORITMO),parts.nombre));
	}
	// argumentos
	std::string str_args = parts.args; Trim(str_args);
	if (not str_args.empty())	{
		if (str_args[0]!='(') { // si no habia argumentos no tiene que haber nada
			if (es_proceso) err_handler.SyntaxError(252,"Se esperaba el fin de linea."); 
			else {
				if (the_func->nombres[0].size()) err_handler.SyntaxError(253,"Se esperaba la lista de argumentos, o el fin de linea.");
				else err_handler.SyntaxError(254,"Se esperaba la lista de argumentos, el signo de asignación, o el fin de linea.");
			}
		} else { // analizar los argumentos
			if (es_proceso) err_handler.SyntaxError(246,MkErrorMsg("El $ principal no puede recibir argumentos.",kw2str(KW_ALGORITMO)));
			int p2 = matchParentesis(str_args,0);
			if (p2==-1) {
				err_handler.SyntaxError(250,"Falta cerrar lista de argumentos.");
			} else {
				if (p2+1!=str_args.size())
					err_handler.SyntaxError(251,"Se esperaba fin de linea.");
				std::string args = str_args.substr(1,p2-1);
				auto vargs = splitArgsList(args); 
				for (std::string &arg : vargs) {
					Trim(arg);
					bool por_copia       = RightCompare(arg,lang.keywords[KW_POR_COPIA],true);
					bool por_referencia = (not por_copia) and RightCompare(arg,lang.keywords[KW_POR_REFERENCIA],true);
					if (arg.empty()) err_handler.SyntaxError(247,"Falta nombre de argumento.");
					else {
						int p = 0;
						if (arg!=NextToken(arg,p))
							err_handler.SyntaxError(249,MkErrorMsg("Se esperaba coma(,) o tipo de pasaje ($ o $).",kw2str(KW_POR_COPIA),kw2str(KW_POR_REFERENCIA)));
						else 
							CheckVariable(rt,arg);
					}
					the_func->AddArg(arg);
					if (por_referencia or por_copia) // si no dice, dejar en desconocido.. porque para arreglos debe deducirse luego referencia y para los demas copia
						the_func->SetLastPasaje(por_referencia?PP_REFERENCIA:PP_VALOR);
				}
			}
		}
	}
	return the_func;
}

FuncStrings SepararCabeceraDeSubProceso(std::string cadena) {
	FuncStrings ret;
	if (not cadena.empty()) {
		if (cadena[0]=='(') { 
			ret.args = cadena;
		} else {
			int p_args = 0; 
			ret.nombre = NextToken(cadena,p_args);
			int p_aux = p_args;
			std::string tok = NextToken(cadena,p_aux);
			if (tok=="="||tok=="<-"||tok==":=") { 
				ret.ret_id = std::move(ret.nombre); 
				ret.nombre = (p_aux==int(cadena.size())||cadena[p_aux]=='(') ? "" : NextToken(cadena,p_aux);
				p_args = p_aux;
			}
			ret.args = cadena.substr(p_args);
		}
	}	
	if (ret.nombre.empty()) ret.nombre = "<sin_titulo_"+std::to_string(Inter.GetLocation().linea)+">";
	return ret;
}

bool TooManyDigits(const std::string &s) {
	auto p=s.begin(), e=s.end();
	while(p!=e and (*p<'1' or *p>'9')) 
		++p;
	int d = 0, z = 0;
	while (p!=e) {
		if (*p>='0' and *p<='9') ++d;
		++p;
	}
	return d>16;
}

bool IsInteger(double d) {
	return d == std::floor(d);
}

