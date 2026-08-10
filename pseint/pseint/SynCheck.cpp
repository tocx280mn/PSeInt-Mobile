#include <deque>
#include <string>
#include "SynCheck.hpp"
#include "RunTime.hpp"
#include "global.h"
#include "utils.h"
#include "Evaluar.hpp"
#include "intercambio.h"
#include "Programa.hpp"
#include "FuncsManager.hpp"
#include "case_map.h"
#include "debug.h"
#include "ErrorHandler.hpp"
#include "strFuncs.hpp"

// ULTIMO NRO DE ERROR UTILIZADO: 329

static int PSeudoFind(const std::string &s, char x, int from=0, int to=-1) {
	if (to==-1) to=s.size();
	if (x=='\"') x='\'';
	else if (x=='[') x='(';
	else if (x==']') x=')';
	int par=0; bool com=false;
	for (int i=from;i<to;i++) {
		char c=s[i];
		if (c=='\''||c=='\"') { 
			if (x=='\'') return i;
			com=!com;
		} else if (!com) {
			if (c=='('||c=='[') {
				if (par==0 && x=='(') return i;
				par++;
			} else if (c==')'||c==']') {
				if (par==0 && x==')') return i;
				par--;
			} else if (par==0 && c==x) 
				return i; 
		}
	}
	return -1;
}

// para checkear que las dimensiones de los arreglos no involucren variables
static bool IsNumericConstant(const std::string &str) {
	for (char c:str)
		if (EsLetra(c)) return false;
	return true;
}


// pasar todo a mayusculas, reemplazar tabs, comillas, word_operators, corchetes, y quita espacios extras
std::pair<std::string,bool> Normalizar(std::string &cadena) {
	std::string rest; bool is_comment = true;
	// corregir saltos de linea win/linux
	if (cadena.size()>0 && (cadena[cadena.size()-1]==13||cadena[cadena.size()-1]==10) ) cadena[cadena.size()-1]=' ';
	if (cadena.size()>1 && (cadena[cadena.size()-2]==13||cadena[cadena.size()-2]==10) ) cadena[cadena.size()-2]=' ';
	// primero, todo a mayúsculas y cambio de comillas y paréntesis
	auto NormalizeChar = lang[LS_ALLOW_ACCENTS]?Normalize:NormalizeKA; // parece al reves... pero es adrede, si no se permiten acentos, los dejo para que luego salte como error de identificador no valido
	for (int i=0,len = cadena.size(); i<len; ++i) {
		char &c=cadena[i];
		if (c==';' && i+1<len) {
			is_comment = false;
			rest = cadena.substr(i+1);
			cadena.erase(i+1);
			break;
		}
		if (i>0 && c=='/' && cadena[i-1]=='/') { // "remover" comentarios
//			if (preserve_comments) {
				// is_comment = true estaba por defecto
				rest = cadena.substr(i+1);
				Trim(rest);
//			}
			cadena = cadena.substr(0,i-1); break; 
		}
		if (c=='\"' || c=='\'') { // saltear cadenas literales normalizando las comillas a '
			c='\'';
			while ((++i)<len&&(cadena[i]!='\"'&&cadena[i]!='\''));
			if (i<len) cadena[i]='\'';
		} else { // normalizar lo que queda
			if (c=='[') c='(';
			else if (c==']') c=')';
			else if (c==9) c=' ';
			else if (c==' ' && i!=0 && cadena[i-1]==' ') { cadena.erase(--i,1); len--; }
			else c = NormalizeChar(c);
		}
	}
	// despues, word_operators
	if (lang[LS_WORD_OPERATORS]) {
		for (int i=0,len=cadena.size(); i<len; ++i) {
			char &c = cadena[i];
			if (c=='\'') { // saltera cadenas literales
				while (i<len&&cadena[++i]!='\'');
			} else {
				if (c=='Y' && (i==0 || !parteDePalabra(cadena[i-1])) && (i==len-1 || !parteDePalabra(cadena[i+1])) )
					c='&';
				if (c=='O' && (i==0 || !parteDePalabra(cadena[i-1])) && (i==len-1 || !parteDePalabra(cadena[i+1])) )
					c='|';
				if (c=='O' && i>0 && cadena[i-1]=='N' && (i-1==0 || !parteDePalabra(cadena[i-2])) && (i+1==len || !parteDePalabra(cadena[i+1])) )
				{ cadena[i-1]='~'; cadena.erase(i,1); i--; len--; }
				if (c=='D' && i>1 && cadena[i-1]=='O' && cadena[i-2]=='M' && (i-2==0 || !parteDePalabra(cadena[i-3])) && (i+1==len || !parteDePalabra(cadena[i+1])) )
				{ cadena[i-2]='%'; cadena.erase(i-1,2); i-=2; len-=2;	}
			}
		}
	}
	// Borrar espacios en blanco al principio y al final
	Trim(cadena);
	// agregar espacios para evitar que cosas como "SI(x<2)ENTONCES" generen un error
//	comillas=false;
//	for(int i=0;i<len;i++) {
//		if (cadena[i]=='\'') comillas=!comillas;
//		else if (!comillas) {
//			if (cadena[i]=='(' && i>0 && EsLetra(cadena[i-1])) { cadena.insert(i," "); len++; }
//			if (cadena[i]==')' && i+2<len && EsLetra(cadena[i+1])) { cadena.insert(i+1," "); len++; }
//		}
//	}
	return std::make_pair(rest,is_comment);
}

struct coloquial_aux {
	std::string cond, pre, post, rep;
	int csize;
	bool binary;
	coloquial_aux(){}
	coloquial_aux(std::string c, std::string pr, std::string re, std::string po) 
		: cond(c),pre(pr),post(po),rep(re), csize(cond.size()), binary(po.size()) {}
	coloquial_aux(std::string c, std::string pr, std::string re, std::string po, bool bin) 
		: cond(c),pre(pr),post(po),rep(re), csize(cond.size()), binary(bin) {}
};

std::vector<coloquial_aux> &GetColoquialConditions() {
	static std::vector<coloquial_aux> v;
	if (v.empty()) {
		v.push_back(coloquial_aux("ES ENTERO ",				"(",	")=TRUNC(",		"<PRE>)"	,false));
		v.push_back(coloquial_aux("ES ENTERA ",				"(",	")=TRUNC(",		"<PRE>)"	,false));
		v.push_back(coloquial_aux("ES MENOR A ",			"(",	")<(",			")"			));
		v.push_back(coloquial_aux("ES MENOR QUE ",			"(",	")<(",			")"			));
		v.push_back(coloquial_aux("ES MAYOR A ",			"(",	")>(",			")"			));
		v.push_back(coloquial_aux("ES MAYOR QUE ",			"(",	")>(",			")"			));
		v.push_back(coloquial_aux("ES IGUAL | MAYOR A ",	"(",	")>=(",			")"			));
		v.push_back(coloquial_aux("ES IGUAL | MAYOR QUE ",	"(",	")>=(",			")"			));
		v.push_back(coloquial_aux("ES IGUAL | MENOR A ",	"(",	")<=(",			")"			));
		v.push_back(coloquial_aux("ES IGUAL | MENOR QUE ",	"(",	")<=(",			")"			));
		v.push_back(coloquial_aux("ES MAYOR | IGUAL A ",	"(",	")>=(",			")"			));
		v.push_back(coloquial_aux("ES MAYOR | IGUAL QUE ",	"(",	")>=(",			")"			));
		v.push_back(coloquial_aux("ES MENOR | IGUAL A ",	"(",	")<=(",			")"			));
		v.push_back(coloquial_aux("ES MENOR | IGUAL QUE ",	"(",	")<=(",			")"			));
		v.push_back(coloquial_aux("ES IGUAL A ",			"(",	")=(",			")"			));
		v.push_back(coloquial_aux("ES IGUAL QUE ",			"(",	")=(",			")"			));
		v.push_back(coloquial_aux("ES DISTINTO A ",			"(",	")<>(",			")"			));
		v.push_back(coloquial_aux("ES DISTINTO DE ",		"(",	")<>(",			")"			));
		v.push_back(coloquial_aux("ES DISTINTA A ",			"(",	")<>(",			")"			));
		v.push_back(coloquial_aux("ES DISTINTA DE ",		"(",	")<>(",			")"			));
		v.push_back(coloquial_aux("ES PAR ",				"(",	")%2=0 ",		""			));
		v.push_back(coloquial_aux("ES IMPAR ",				"(",	")%2=1 ",		""			));
		v.push_back(coloquial_aux("ES POSITIVO ",			"(",	")>0 ",			""			));
		v.push_back(coloquial_aux("ES POSITIVA ",			"(",	")>0 ",			""			));
		v.push_back(coloquial_aux("ES NEGATIVO ",			"(",	")<0 ",			""			));
		v.push_back(coloquial_aux("ES NEGATIVA ",			"(",	")<0 ",			""			));
		v.push_back(coloquial_aux("ES CERO ",				"(",	")=0 ",			""			));
		v.push_back(coloquial_aux("ES DIVISIBLE POR ",		"(",	") % (",		")=0"		));
		v.push_back(coloquial_aux("ES MULTIPLO DE ",		"(",	") % (",		")=0"		));
		v.push_back(coloquial_aux("ES ",					"(",	")=(",			")"			));
	}
	return v;
}
	
// reescribir condiciones coloquiales
void Condiciones(RunTime &rt, std::string &cadena) {
	ErrorHandler &err_handler = rt.err;
	if (!cadena.size() || !lang[LS_COLOQUIAL_CONDITIONS]) return;
	if (not LastCharIs(cadena,' ')) cadena += ' ';
	bool comillas=false;
	for (int y=0;y<int(cadena.size());y++) {
		if (cadena[y]=='\'' || cadena[y]=='\"') comillas=!comillas;
		else if (!comillas) {
			if (y>0&&(cadena[y]==','||/*cadena[y]=='('||*/cadena[y]==';')&&cadena[y-1]!=' ')
				cadena.insert(y++," ");
		}
	}
	
	std::vector<coloquial_aux> &coloquial_conditions_list = GetColoquialConditions();
	for (int y=0;y<int(cadena.size());y++) {
		if (cadena[y]=='\'' || cadena[y]=='\"') {
			while (++y<int(cadena.size()) && cadena[y]!='\'' && cadena[y]!='\"');
			continue;
		}	
		if (y+2>=int(cadena.size()) || (y>0&&cadena[y-1]!=' '&&cadena[y-1]!=',')||cadena[y]!='E'||cadena[y+1]!='S'||cadena[y+2]!=' ') continue;
		// buscar si coincide con alguna expresión de la lista
		int cual=-1;
		for(size_t j=0;j<coloquial_conditions_list.size();j++) { 
			coloquial_aux &col = coloquial_conditions_list[j];
			if (cadena.substr(y,col.csize)==col.cond) {	cual=j;	break;}
		}
		if (cual==-1) continue; // si no era ninguna de las expresiones coloquiales
		// ver si decia "NO ES" en lugar de "ES"
		bool negate=(y>=2 && cadena[y-2]=='~'); 
		if (negate) { cadena.erase(y-2,2); y-=2; }
		// elegir la condición y reemplazarla por su operador
		coloquial_aux &col=coloquial_conditions_list[cual];
		cadena.replace(y,col.csize,col.rep);
		// agregar pre y post antes y despues de los operandos
		std::string pre;
		{
			int parentesis=0, yold=y--; bool comillas=false;
			while ( y>=0 && ( (parentesis>0||comillas) || (cadena[y]!='&' && cadena[y]!=',' && cadena[y]!='|' && cadena[y]!='~' && cadena[y]!='(') ) ) {
				if (cadena[y]=='\''||cadena[y]=='\"') comillas=!comillas;
				else if (!comillas) {
					if (cadena[y]==')') parentesis++;
					else if (cadena[y]=='(') parentesis--;
				}
				y--;
			}
			int y2=yold-1; while (y2>y && cadena[y2]==' ') y2--;
			
			pre=cadena.substr(y+1,y2-y);
			if (pre.empty()) err_handler.SyntaxError(317,MkErrorMsg("Falta operando (antes de la condición coloquial $).",col.cond));
			if (col.pre.size()) cadena.insert(y+1,col.pre);
			if (negate) { cadena.insert(y+1,"~"); yold++; }
			y=yold+col.pre.size();
		}
		{
			int parentesis=0, yold=y,l=cadena.size(); int y0 = y+=col.rep.size();  bool comillas=false;
			while ( y<l 
				   && ( (parentesis>0||comillas) || (cadena[y]!='&' && cadena[y]!=',' && cadena[y]!='|' && cadena[y]!='~' && cadena[y]!=')' && cadena[y]!=';'))
				   && (comillas||(cadena.substr(y,2)!="//"&&cadena.substr(y,10)!=" ENTONCES "&&cadena.substr(y,7)!=" HACER ")) ) {
				if (cadena[y]=='\''||cadena[y]=='\"') comillas=!comillas;
				else if (!comillas) {
					if (cadena[y]==')') parentesis--;
					else if (cadena[y]=='(') parentesis++;
				}
				y++;
			}
			while (y>yold && cadena[y-1]==' ') y--;
			if (col.binary) {
				if (y0>=y) err_handler.SyntaxError(318,MkErrorMsg("Falta operando (después de la condición coloquial $).",col.cond));
			} else {
				if (y0<y) err_handler.SyntaxError(319,MkErrorMsg("No corresponde operando (después de la condición coloquial $).",col.cond));
			}
			std::string post=col.post;
			size_t n=post.find("<PRE>");
			if (n!=std::string::npos) post.replace(n,5,pre);
			cadena.insert(y,post);
			y=yold;
		}
	} // for y=0...cadena.size()
}

enum what { w_null, w_operand, w_operator, w_space };
enum what_extra { w_comma, w_other, 
	w_relational_op,w_logic_op,w_aritmetic_op,
	w_number_int, w_number_dec, w_string, w_id, w_expr,
	w_binary_op, w_unary_op, w_ambiguos_op, w_no_op };


// retorna 1 si es operador unario, 2 si es binario, 3 si puede ser cualquiera de los dos, y 0 si no es operador
// ademas, en len retorna cuando ocupa ese operador
int is_valid_operator(char act, char next, int &len, what_extra &type) {
	switch (act) {
		case '+': case '-': 
			len=1; type=w_aritmetic_op; return w_ambiguos_op; 
		case '*': case '/': case '^': case '%':
			len=1; type=w_aritmetic_op; return w_binary_op; 
		case '~':  
			len=1; type=w_logic_op; return w_unary_op;
		case '&': case '|':
			len=(next==act)?2:1; 
			type=w_logic_op; return w_binary_op;
		case '!':
			type=w_relational_op; 
			if (next=='=') { len=2; return w_binary_op; }
			else { len=1; return w_unary_op; }
		case '<': case '>': case '=':
			type=w_relational_op; 
			len=(next=='='||(act=='<'&&next=='>'))?2:1;
			return w_binary_op;
	}
	return w_no_op;
}

// verificar operadores, constantes y parentesis, y borrar los espacios en blanco que sobran entre ellos
void Operadores(RunTime &rt, const int &x, std::string &cadena, InstructionType instruction_type) {
	ErrorHandler &err_handler = rt.err;
	bool allow_multiple_expresions=instruction_type!=IT_DEOTROMODO && instruction_type!=IT_ASIGNAR;
	what w=w_null; what_extra wext=w_other;
	int parentesis=0, csize; bool comillas=false;
	for (int i=0;i<(csize=(int)cadena.size());i++) {
		char act=cadena[i]; 
		if (act=='\'') {
			if (!comillas && w==w_operand) err_handler.SyntaxError(304,"Falta operador (antes de la cadena de caracteres).");
			else if (comillas) { w=w_operand; wext=w_string; }
			comillas=!comillas;
		} else if (!comillas) {
			if (act==' ') {
				char next=cadena[i+1]; bool next_es_letra=EsLetra(next);
				if (w==w_operator) {
					// solo puede seguir un operando (id, cte, o expresión)
					if (next_es_letra) {
						int j=i+2; while (EsLetra(cadena[j],true)) j++;
						const std::string word=cadena.substr(i+1,j-i-1);
						if (PalabraReservada(word) && word!=VERDADERO && word!=FALSO) 
							err_handler.SyntaxError(237,MkErrorMsg("Falta operando (antes de $).",cadena.substr(i+1,j-i-1))); // hola+ ;
						else { cadena.erase(i,1); i--; }
					} else if ((next>='0'&&next<='9') || next=='.' || next=='\'' || next=='(' || next==':' || next==';') {
						cadena.erase(i,1); i--;
					} else {
						what_extra type; int len; int ret=is_valid_operator(cadena[i+1],i+2<csize?cadena[i+2]:' ',len,type);
						if (ret!=w_unary_op&&ret!=w_ambiguos_op) 
							err_handler.SyntaxError(224,MkErrorMsg("Falta operando (despues de $).",std::string(1,cadena[i-1])));
						else { cadena.erase(i,1); i--; }
					}
				} else if (w==w_operand) {
					// si lo que sigue es otro operador, puede haber problemas a menos que se trate de "escribir" con sintaxis flexible
					if (next==')' || next==':' || next==';') {
						cadena.erase(i,1); i--;
					} else if (next_es_letra || (next>='0'&&next<='9') || next=='.' || next=='\'' || ((next=='('||next=='~') && allow_multiple_expresions)) {
						if (allow_multiple_expresions) {
							w=w_null;
//							cadena.erase(i,1); i--;
						} else {
							err_handler.SyntaxError(225,"Falta operador o coma entre operandos.");
						}
					} else {
						cadena.erase(i,1); i--;
					}
				} else {
					cadena.erase(i,1); i--;
				}
			} else if (act==';' || act==':') {
				if (act==':' && instruction_type!=IT_OPCION) err_handler.SyntaxError(226,"Operador no válido (:).");
				else if (w==w_operator) 
					err_handler.SyntaxError(227,MkErrorMsg("Falta operando (antes de $).",std::string(1,act)));
				w=w_null; wext=w_other;
				if (act==';') {
					// todo: ver si realmente puede llegar esto hasta aca, o se corta en otro lado
//					programa.Insert(x+1,cadena.substr(i+1));
					_expects(i+2==csize);
				}
			} else if (act==',') {
				if (w==w_operator) err_handler.SyntaxError(228,"Falta operador.");
				else if (w==w_null && wext==w_comma) err_handler.SyntaxError(3,"Parámetro nulo."); // 35+;
				w=w_null; wext=w_comma;
			
			} else if (act=='(') {
				if (w==w_operand && wext!=w_id) err_handler.SyntaxError(229,"Falta operador."); // 123(21
				w=w_null; wext=w_other; parentesis++;
			
			} else if (act==')') {
				if (w==w_operator) err_handler.SyntaxError(230,"Falta operando (antes de ')').");
				w=w_operand; wext=w_expr; parentesis--;
			
			} else if (act>='0'&&act<='9') {
				if (w!=w_operand) { w=w_operand; wext=w_number_int; }
				else if (wext==w_string) err_handler.SyntaxError(305,"Falta operador (despues de la cadena de caracteres).");
				else if (wext==w_expr) err_handler.SyntaxError(239,"Falta operador (despues de ')').");
			
			} else if (act=='.') {
				if (w!=w_operand) { w=w_operand; wext=w_number_dec; }
				else if (wext!=w_number_int) {
					if (!ignore_logic_errors) err_handler.SyntaxError(231,"Constante numérica no válida.");
				} else wext=w_number_dec;
			
			} else if (EsLetra(act)) {
				if (w==w_operand && wext!=w_id) {
					if (wext==w_string)
						err_handler.SyntaxError(233,"Falta operando (después de cadena de texto).");
					else if (wext==w_expr)
						err_handler.SyntaxError(307,"Falta operando (posiblemente después de ')').");
					else
						if (!ignore_logic_errors) err_handler.SyntaxError(238,"Constante numérica no válida.");
				}
				w=w_operand; wext=w_id;
			
			} else {
				int len=1; char next=cadena[i+1];
				what_extra op_type;
				int ret=is_valid_operator(act,next,len,op_type);
				if (ret!=w_no_op) {
					if (ret==w_binary_op) {
						if (w!=w_operand) err_handler.SyntaxError(234,MkErrorMsg("Falta operando (antes de $).",cadena.substr(i,len)));
						i+=len-1;
					} else {
						if (w==w_operator && wext==w_aritmetic_op) err_handler.SyntaxError(235,MkErrorMsg("Falta operando (antes de $).",cadena.substr(i,len)));
					}
					w=w_operator; wext=op_type;
				} else {
					if (i==0 or (act!=cadena[i-1])) // si encuentra algo como "???" que marque un solo ?
						err_handler.SyntaxError(68,MkErrorMsg("Caracter no válido ($).",std::string(1,act)));
					w = w_operand; // fuerzo a operando para que preserve espacios despues de esto (casos como "mientras ??? hacer", que no pegue ??? con hacer porque eso generar un error adicional "falta hacer").
				}
			}
		}
	}
	if (w==w_operator) err_handler.SyntaxError(236,"Falta operando al final de la expresión");
	// Posibles errores encontrados
	if (parentesis<0) err_handler.SyntaxError(35,"Se cerraron parentesis o corchetes demás.");
	if (parentesis>0) err_handler.SyntaxError(36,"Falta cerrar parentesis o corchete.");
	if (comillas) err_handler.SyntaxError(37,"Falta cerrar comillas.");
}

static void FixAcentos(std::string &s) {
	for(char &c : s) {
		if (c=='Ñ') c='N';
		else if (c=='Á') c='A';
		else if (c=='É') c='E';
		else if (c=='Í') c='I';
		else if (c=='Ó') c='O';
		else if (c=='Ú') c='U';
		else if (c=='Ü') c='U';
	}
}

void InformUnclosedLoops(RunTime &rt, std::vector<int> &bucles) {
	ErrorHandler &err_handler = rt.err;
	Programa &programa = rt.prog;
	// Controlar Cierre de Bucles
	while (!bucles.empty())	{
		InstructionType type = programa[bucles.back()].type;
		CodeLocation loc = programa[bucles.back()].loc;
		if      (type==IT_PARA || type==IT_PARACADA) err_handler.SyntaxError(114,"Falta cerrar PARA.",loc);
		else if (type==IT_REPETIR) err_handler.SyntaxError(115,"Falta cerrar REPETIR.",loc);
		else if (type==IT_MIENTRAS) err_handler.SyntaxError(116,"Falta cerrar MIENTRAS.",loc);
		else if (type==IT_SI || type==IT_SINO) err_handler.SyntaxError(117,"Falta cerrar SI.",loc);
		else if (type==IT_SEGUN) err_handler.SyntaxError(118,"Falta cerrar SEGUN.",loc);
		else if (type==IT_PROCESO) {
			if (getImpl<IT_PROCESO>(programa[bucles.back()]).principal)
				err_handler.SyntaxError(119,"Falta cerrar ALGORITMO/PROCESO.",loc);
			else
				err_handler.SyntaxError(119,"Falta cerrar FUNCION/SUBPROCESO.",loc);
		}
		bucles.pop_back();
	}
}

bool SirveParaReferencia(RunTime &rt, const std::string &s) {
	int p=0, l=s.size(), parentesis=0;
	bool in_name=true; // si estamos en la primer parte (nombre) o segunda (indices si es un arreglo)
	while (p<l) {
		if (s[p]=='(') {
			if (in_name) {
				if (!CheckVariable(rt,s.substr(0,p))||rt.funcs.IsFunction(s.substr(0,p))) return false;
				in_name=false;
			}
			parentesis++;
		} else if (s[p]==')') {
			parentesis--;
		} else if (parentesis==0 && !in_name) return false; // cualquier operador fuera de parentesis indica expresión y no es legal
		p++;
		
	}
	if (in_name && (!CheckVariable(rt,s)||rt.funcs.IsFunction(s)) ) return false;
	return true;
}

//-------------------------------------------------------------------------------------------
// ********************* Checkear la Correcta Sintaxis del Archivo **************************
//-------------------------------------------------------------------------------------------

void Instrucciones(RunTime &rt) {
	
	Programa &programa = rt.prog;
	ErrorHandler &err_handler = rt.err;
	auto kw2str = [&keys=lang.keywords](int i) { return keys[i].get(); };
	
	Memoria global_memory(NULL); // para usar al analizar instrucciones fuera de proceso/subprocesos
	memoria=&global_memory;
	std::vector<int> bucles; // Para controlar los bucles que se abren y cierran... guarda un stack de indices a instrucciones que abren bloques en el arreglo programa
	bool in_process=false;
	Funcion *current_func = NULL; // funcion actual, necesito el puntero para setear line_end cuando encuentre el FinProceso/FinSubProceso
	
	// Checkear sintaxis y reorganizar el codigo
	for (int prog_idx=0; prog_idx<programa.GetInstCount(); ++prog_idx){
		Inter.SetLocation(programa[prog_idx].loc);
		std::string cadena = programa[prog_idx].instruccion;
		auto this_instruction_is_ok = 
			[prev_count=err_handler.GetErrorsCount(),&err_handler](){
				return prev_count==err_handler.GetErrorsCount();
			};
		
		{
			
//			Instruccion &inst = programa[prog_idx];
			#define inst programa[prog_idx] // no usar referencia porque al modificar el vector se invalida
			
			// puede haber que trimear las cadenas que surgieron de separar lineas con mas de una instrucción
			Trim(cadena);
			
			if (inst.type==IT_COMMENT) continue;
			_expects(inst.type==IT_NULL);
			
			// extraer la primera palabra clave para ver qué instrucción es
			auto first = BestMatch(lang.keywords,cadena, true);
			auto first_word_id = first.first; auto &first_word_str = first.second;
			
			// en esta parte (chorrera de if {} else if {} else if...) se identifica la instrucción... 
			//       primero se busca si empieza con alguna palabra clave
			//       sino, se mira si puede ser opcion de un segun viendo si estamos en un segun y hay : en la cadena
			//       sino, se mira si puede asignacion buscando alguno de los operadores de asignacion
			//       sino, se mira si puede ser la nueva definicion (x es entero), mirando si la anteultima palabra es ES o SON
			// si se identifica la instrucción, se quita del string cadena y se guarda en el string instrucción
			if (first_word_id==KW_ENTONCES) {
				if (/*programa[bucles.back()]!=IT_SI || */programa[prog_idx-1]!=IT_SI)
					err_handler.SyntaxError(1,MkErrorMsg("$ mal colocado.",first_word_str));
				if (not cadena.empty()) programa.Insert(prog_idx+1,cadena);
				inst.setType(IT_ENTONCES); cadena="";
			} else if (first_word_id==KW_SINO) {
				if (not cadena.empty()) programa.Insert(prog_idx+1,cadena);
				inst.setType(IT_SINO); cadena="";
			} else if (first_word_id==KW_ESCRIBIR) {
				inst.setType(IT_ESCRIBIR);
				if (FindKeyword(cadena,lang.keywords[KW_SIN_SALTAR],true)!=-1)
					std::get<Instruccion::IEscribir>(inst.impl).saltar = false;
			} else if (first_word_id==KW_LEER) {
				inst.setType(IT_LEER);
			} else if (first_word_id==KW_SI) {
				inst.setType(IT_SI);
				bucles.push_back(prog_idx);
				// cortar el entonces si esta en la misma linea
				if (RightCompare(cadena,lang.keywords[KW_ENTONCES],true)) {
					programa.Insert(prog_idx+1,lang.keywords[KW_ENTONCES].get(true));
				} else {
					int p = FindKeyword(cadena,lang.keywords[KW_ENTONCES],true);
					if (p!=-1) {
						programa.Insert(prog_idx+1,cadena.substr(p));
						cadena.erase(p);
						RightTrim(cadena); /// @todo: revisar si esto es necesario
					}
				}
			} else if (first_word_id==KW_MIENTRAS) { 
				inst.setType(IT_MIENTRAS);
				bucles.push_back(prog_idx);
			} else if (first_word_id==KW_SEGUN) {
				inst.setType(IT_SEGUN);
				bucles.push_back(prog_idx);
			} else if (first_word_id==KW_DEOTROMODO) {
				if (bucles.empty() || programa[bucles.back()]!=IT_SEGUN) err_handler.SyntaxError(321,MkErrorMsg("$ mal colocado.",first_word_str));
				programa.Insert(prog_idx+1,cadena.substr(1)); // cortar los ':'
				inst.setType(IT_DEOTROMODO); cadena="";
			} else if (first_word_id==KW_DIMENSIONAR or first_word_id==KW_REDIMENSIONAR) {
				inst.setType(IT_DIMENSION);
				getImpl<IT_DIMENSION>(inst).redimension = first_word_id==KW_REDIMENSIONAR;
			} else if (first_word_id==KW_HASTAQUE) {
				inst.setType(IT_HASTAQUE);
			} else if (first_word_id==KW_MIENTRASQUE) {
				inst.setType(IT_HASTAQUE); 
				getImpl<IT_HASTAQUE>(inst).mientras_que = true;
			} else if (first_word_id==KW_FINSI) {
				inst.setType(IT_FINSI);
			} else if (first_word_id==KW_FINPARA) {
				inst.setType(IT_FINPARA);
			} else if (first_word_id==KW_FINMIENTRAS) {
				inst.setType(IT_FINMIENTRAS);
			} else if (first_word_id==KW_FINSEGUN) {
				inst.setType(IT_FINSEGUN);
			} else if (first_word_id==KW_ESPERARTECLA) {
				inst.setType(IT_ESPERARTECLA);
			} else if (first_word_id==KW_ESPERARTIEMPO) {
				inst.setType(IT_ESPERAR);
			} else if (first_word_id==KW_LIMPIARPANTALLA) {
				inst.setType(IT_BORRARPANTALLA);
			} else if (first_word_id==KW_ALGORITMO) {
				inst.setType(IT_PROCESO); getImpl<IT_PROCESO>(inst).principal = true;
			} else if (first_word_id==KW_SUBALGORITMO) {
				inst.setType(IT_PROCESO); getImpl<IT_PROCESO>(inst).principal = false;
			} else if (first_word_id==KW_PARACADA) {
				inst.setType(IT_PARACADA);
				bucles.push_back(prog_idx);
			} else if (first_word_id==KW_PARA) {
				inst.setType(IT_PARA);
				bucles.push_back(prog_idx);
				// si se puede asignar con igual, reemplazar aca
				if (lang[LS_OVERLOAD_EQUAL]) {
					int i=0, l=cadena.size();
					while (i<l && EsLetra(cadena[i])) i++;
					while (i<l && (cadena[i]==' '||cadena[i]=='\t')) i++;
					if (i<l&& cadena[i]=='=') cadena.replace(i,1,"<-");
				}
				// evitar problema de operador incorrecto al poner el signo al numero
				// si dice "i desde 1" en lugar de "i<-1" se reemplaza " desde " por "<-"
				size_t pos_arrow = cadena.find("<-",0);
//				if (lang[LS_LAZY_SYNTAX] && pos_arrow==std::string::npos) {
//					int p = FindKeyword(cadena,lang.keywords[KW_DESDE],true);
//					if (p!=-1 and p!=0) { cadena.insert(p,"<-"); pos_arrow = p; }
//				}
				if (pos_arrow!=std::string::npos) {
					// se agregan parentesis al valor inicial para evitar problemas mas adelante (porque si el valor es negativo, con la flecha de asignacion queda un --)
					int p1 = FindKeyword(cadena,lang.keywords[KW_HASTA],false);
					int p2 = FindKeyword(cadena,lang.keywords[KW_CONPASO],false);
					int p = (p1!=-1 and p2!=-1) ? std::min(p1,p2) : (p1==-1?p2:p1);
					if (p!=-1) {
						if (p!=0 and cadena[p-1]==' ') --p;
						cadena.insert(p,")");
						cadena.insert(pos_arrow+2,"(");
					}
				}
			} else if (first_word_id==KW_FINALGORITMO) {
				inst.setType(IT_FINPROCESO); getImpl<IT_FINPROCESO>(inst).principal = true;
				if (!ignore_logic_errors&&cadena==";") err_handler.SyntaxError(315,MkErrorMsg("$ no lleva punto y coma.",first_word_str));
			} else if (first_word_id==KW_FINSUBALGORITMO) {
				inst.setType(IT_FINPROCESO); getImpl<IT_FINPROCESO>(inst).principal = false;
				if (!ignore_logic_errors&&cadena==";") err_handler.SyntaxError(315,MkErrorMsg("$ no lleva punto y coma.",first_word_str));
			} else if (first_word_id==KW_REPETIR) {
				inst.setType(IT_REPETIR); bucles.push_back(prog_idx);
			} else if (first_word_id==KW_DEFINIR) {
				inst.setType(IT_DEFINIR);
			} else if (first_word_id==KW_OPCION) {
				inst.setType(IT_OPCION);
				int pos_dp = PSeudoFind(cadena,':');
				if (pos_dp==-1) {
					err_handler.SyntaxError(326,"Se esperan dos puntos (:) luego de la lista de opciones.");
					cadena += ":";
				} else { 
					// ver ademas si dice "OPCION o CASO o similar al principio" y eliminarlo
					programa.Insert(prog_idx+1,cadena.substr(pos_dp+1));
					cadena.erase(pos_dp+1);
				}
			} else {
				// no se identifica el tipo por la primera palabra clave... 
				// puede ser asignación (buscar <-, = o :=), opción de segun (buscar :) 
				// definicion (buscar ES), o invocacion a subalgoritmo
				cadena = inst.instruccion;
				
				// opcion del segun?
				int pos_dp = PSeudoFind(cadena,':');
				if (pos_dp!=-1 && cadena[pos_dp+1]!='=') { // no es asignacion estilo x:=0
					// ver ademas si dice "OPCION o CASO o similar al principio" y eliminarlo
					inst.setType(IT_OPCION);
					programa.Insert(prog_idx+1,cadena.substr(pos_dp+1));
					cadena.erase(pos_dp+1);
				}
				
				else {
					inst.setType(IT_ERROR);
					// saltar primera cosa (puede ser un id de arreglo con indices y todo)
					int i=0, l=cadena.size();
					while (i<l && (cadena[i]=='\t'||cadena[i]==' ')) i++;
					int par=0;
					while (i<l && (par||
//						(cadena[i]>='a'&&cadena[i]<='z') || // no deberia hacer falta a esta altura
						EsLetra(cadena[i]) ||
						(cadena[i]>='0'&&cadena[i]<='9') ||
						cadena[i]=='_'||cadena[i]=='(')) {
							if (cadena[i]=='(') par++;
							else if (cadena[i]==')') par--;
							i++;
						}
					while (i<l && (cadena[i]=='\t'||cadena[i]==' ')) i++;
					
					// asignacion ??
					if (i>0&&i<l) {
						if (i+1<l && cadena[i]==':' && cadena[i+1]=='=') { cadena[i]='<'; cadena[i+1]='-'; }
						else if (lang[LS_OVERLOAD_EQUAL] && cadena[i]=='=') cadena.replace(i,1,"<-");
						if (i+1<l && cadena[i]=='<' && cadena[i+1]=='-') {
							bool comillas = false;
							for (int y=0;y<i;y++)
								if(cadena[y]=='\"' || cadena[y]=='\'') comillas=!comillas;
							if (!comillas) {
								inst.setType(IT_ASIGNAR);
								// evitar problema de operador incorrecto al poner el signo al numero
								cadena.insert(i+2,"(");
								cadena.insert(cadena.size(),") ");
								for (int y=i+3; y<(int)cadena.size();y++) {
									if(cadena[y]=='\"' || cadena[y]=='\'') comillas=!comillas;
									if ((!comillas) && cadena[y]==';') {
										cadena.insert(y,")");
										cadena.erase(cadena.size()-2,2);
										break;
									}
								}
							} 
						} 
					}
					
					// definicion?
					if ((not cadena.empty()) and lang[LS_LAZY_SYNTAX] and inst.type!=IT_ASIGNAR) { // definición de tipos alternativa (x es entero)
						size_t pos = cadena.rfind(' ',cadena.size()-(LastCharIs(cadena,';')?3:2));
						if (pos!=std::string::npos) {
							int pos = FindKeyword(cadena,lang.keywords[KW_ES],true);
							if (pos!=-1) {
								inst.setType(IT_DEFINIR);
								cadena.insert(pos," "+lang.keywords[KW_COMO].get(true)+" ");
							}
						}
					}
					
					// llamada a subalgoritmo?
					if (inst.type!=IT_ASIGNAR && inst.type!=IT_DEFINIR) {
						int p=0, l=cadena.length();
						while (p<l&&EsLetra(cadena[p],true)) p++;
						if (p!=0 and rt.funcs.IsFunction(cadena.substr(0,p)))
							inst.setType(IT_INVOCAR);
					}
				}
				
			}
			
			// reescribir condiciones coloquiales
			Condiciones(rt,cadena);
			
			// verificar operadores
			Operadores(rt,prog_idx,cadena,inst.type);
			
			/// @todo: ver si poner esto en las estructuras de control que llevan hacer... o ver cómo se resuelve el entonces, que debería ser similar
			// si hay algo a continuacion del hacer se corta como si fuera ; para que se pueda escribir por ejemplo un mientras en una sola linea
			int p_hacer = FindKeyword(cadena,lang.keywords[KW_HACER],true);
			if (p_hacer!=-1) {
				if (p_hacer!=int(cadena.size())) {
					programa.Insert(prog_idx+1, cadena.substr(p_hacer));
					cadena.erase(p_hacer);
				}
				cadena += " "+lang.keywords[KW_HACER].get(true);
			}
			
			if (LastCharIs(cadena,','))
				err_handler.SyntaxError(31,"Parametro nulo.");
			while (cadena[0]==';' && cadena.size()>1) cadena.erase(0,1); // para que caso esta esto?
			// Controlar que el si siempre tenga un entonces
			if (prog_idx&&programa[prog_idx-1]==IT_SI)
				if (inst.type!=IT_ENTONCES && inst.type!=IT_NULL && inst.type!=IT_ERROR) {
					if (lang[LS_LAZY_SYNTAX]) {
						programa.Insert(prog_idx,lang.keywords[KW_ENTONCES].get(true),inst.loc);
						inst.setType(IT_ENTONCES);
						if (bucles.back()==prog_idx) ++bucles.back(); // por si justo se habria otro bloque en la inst actual
						++prog_idx;
					} else 
						err_handler.SyntaxError(32,MkErrorMsg("Falta $.",kw2str(KW_ENTONCES)),Inter.GetPreviousLocation());
				}
			// si entro en segun comprobar que haya opción
			if (!bucles.empty()) {
				if (programa[bucles.back()]==IT_SEGUN && programa[prog_idx-1]==IT_SEGUN && cadena!="") {
					if (inst.type!=IT_OPCION) err_handler.SyntaxError(33,"Se esperaba <opcion>:.");
				}
			}
			
			// Controlar el punto y coma
			bool lleva_pyc = inst.type==IT_DIMENSION || inst.type==IT_DEFINIR ||
				             inst.type==IT_ESCRIBIR ||  
							 inst.type==IT_ASIGNAR || inst.type==IT_LEER || inst.type==IT_ESPERAR || 
							 inst.type==IT_ESPERARTECLA || inst.type==IT_BORRARPANTALLA || inst.type==IT_INVOCAR;
			if (lleva_pyc) {
				if (not LastCharIs(cadena,';')) {
					if (lang[LS_FORCE_SEMICOLON])
						err_handler.SyntaxError(38,"Falta punto y coma.");
					cadena += ';';
				}
			}
			
			// En esta parte, según cada instrucción se verifican si los argumentos están bien. Los argumentos quedaron solos en cadena, la instrucción ya fué cortada.
			if (inst.type==IT_PROCESO) {
				auto &inst_impl = getImpl<IT_PROCESO>(inst);
				if (in_process) InformUnclosedLoops(rt,bucles);
				in_process = true;
				auto func_strs = SepararCabeceraDeSubProceso(cadena);
				inst_impl.nombre = func_strs.nombre;
				inst_impl.ret_id = func_strs.ret_id;
				inst_impl.args = func_strs.args;
				current_func = rt.funcs.GetEditableSub(inst_impl.nombre);
				current_func->line_start = prog_idx;
				bucles.push_back(prog_idx);
				current_func->userline_start = Inter.GetLocation().linea;
				memoria = (current_func->memoria = std::make_unique<Memoria>(current_func)).get();
			}
			if (!in_process && inst.type!=IT_NULL&&cadena!="") {
				err_handler.SyntaxError(43,MkErrorMsg("Instrucción fuera de $.",
													  kw2str(KW_ALGORITMO) +
													  (lang[LS_ENABLE_USER_FUNCTIONS] ? "/"+kw2str(KW_SUBALGORITMO) : "") ));
			}
			if (inst.type==IT_FINPROCESO) {
				auto &inst_impl = getImpl<IT_FINPROCESO>(inst);
				bool sub=!inst_impl.principal; in_process=false;
				if (!bucles.empty()) {
					auto &inst_back = programa[bucles.back()];
					if (inst_back==IT_PROCESO and inst_impl.principal==getImpl<IT_PROCESO>(inst_back).principal) {
						getImpl<IT_PROCESO>(inst_back).fin = prog_idx;
						if (current_func) { 
							inst_impl.nombre = current_func->id;
							if (!current_func->nombres[0].empty()) {
								tipo_var ret_t = memoria->LeerTipo(current_func->nombres[0]);
								current_func->tipos[0].set(ret_t);
							}
							current_func->userline_end=Inter.GetLocation().linea;
							current_func=NULL; 
						}
						bucles.pop_back();
					} else {
						auto &inst_front = programa[bucles.front()];
						if (inst_front==IT_PROCESO and inst_impl.principal==getImpl<IT_PROCESO>(inst_front).principal) {
							getImpl<IT_PROCESO>(inst_front).fin = prog_idx;
							bucles.erase(bucles.begin());
							InformUnclosedLoops(rt,bucles);
						}
					}
				} else {
					err_handler.SyntaxError(308,MkErrorMsg("$ mal colocado.",first_word_str));
				}
				memoria=&global_memory;
			}
			
			// Controlar correcta y completa sintaxis de cada instruccion
			if (inst.type==IT_DEFINIR) {  // ------------ DEFINIR -----------//
				auto &inst_impl = getImpl<IT_DEFINIR>(inst);
				if (cadena=="" || cadena==";") err_handler.SyntaxError(44,"Faltan parámetros.");
				else {
					if (LastCharIs(cadena,';')) EraseLastChar(cadena);
					else if ((not ignore_logic_errors) and lang[LS_FORCE_SEMICOLON]) 
						err_handler.SyntaxError(45,"Falta punto y coma.");
					
					int pos_como = FindKeyword(cadena,lang.keywords[KW_COMO],true), pos_paso = -1;
					if (pos_como==-1) {
						err_handler.SyntaxError(324,MkErrorMsg("Falta $.",kw2str(KW_ES)));
					} else {
						inst_impl.tipo = vt_desconocido;
						std::string str_tipo = cadena.substr(pos_como);
						if (LeftCompare(str_tipo,lang.keywords[KW_TIPO_ENTERO],true))
							inst_impl.tipo = vt_numerica_entera;
						else if (LeftCompare(str_tipo,lang.keywords[KW_TIPO_REAL],true))
							inst_impl.tipo = vt_numerica;
						else if (LeftCompare(str_tipo,lang.keywords[KW_TIPO_CARACTER],true))
							inst_impl.tipo = vt_caracter;
						else if (LeftCompare(str_tipo,lang.keywords[KW_TIPO_LOGICO],true))
							inst_impl.tipo = vt_logica;
						else if (!ignore_logic_errors) 
							err_handler.SyntaxError(46,"Falta tipo de dato o tipo no válido.");
						if (inst_impl.tipo!=vt_desconocido and (not str_tipo.empty()))
							err_handler.SyntaxError(325,"Se esperaba el fin de la instrucción (luego del tipo).");
						
						// evaluar los nombre de variables
						/// @todo: usar funcion auxiliar para cortar listas, aca, en leer, escribir, dimension...
						cadena.erase(pos_como); cadena += ",";
						int parentesis=0, i0=0;
						for (size_t i=0; i<cadena.size(); ++i) {
							if (cadena[i]=='(') parentesis++;
							if (cadena[i]==')') parentesis--;
							if (i>0 && i<(int)cadena.size()-1)
								if (cadena[i]==' ' && cadena[i-1]!='&' && cadena[i-1]!='|'  && cadena[i+1]!='&'  && cadena[i+1]!='|')
									err_handler.SyntaxError(47,"Se esperaba fin de expresión (fin de la instrucción, o coma para separar).");
							if (parentesis==0 && cadena[i]==',') { // comprobar validez
								std::string str=cadena;
								str.erase(i,str.size()-i);
								str.erase(0,i0);
								if (str.find("(",0)==std::string::npos) {
									if (CheckVariable(rt,str,48)) {
										if (memoria->EsArgumento(str) && !ignore_logic_errors) err_handler.SyntaxError(222,"No debe redefinir el tipo de un argumento.");
										memoria->DefinirTipo(str,inst_impl.tipo);
									}
								} else {
									str.erase(str.find("(",0),str.size()-str.find("(",0));
									if (!ignore_logic_errors) err_handler.SyntaxError(212,MkErrorMsg("No debe utilizar subindices ($).",str));
								}
								inst_impl.variables.push_back(str);
								i0=i+1;
							}
						}
					}
				}
			}
			if (inst.type==IT_ESCRIBIR) {  // ------------ ESCRIBIR -----------//
				auto &inst_impl = getImpl<IT_ESCRIBIR>(inst);
				if (cadena=="" || cadena==";") err_handler.SyntaxError(53,"Faltan parámetros.");
				else {
					if (LastCharIs(cadena,';'))
						cadena[cadena.size()-1]=',';
					else
						cadena += ',';
					bool comillas=false; // cortar parámetros
					int parentesis=0;
					for (int last_i=0, i=0;i<(int)cadena.size();i++) {
						if (cadena[i]=='\'') comillas=!comillas;
						else if (comillas) continue;
						if (cadena[i]=='(') parentesis++;
						if (cadena[i]==')') parentesis--;
						if (i>0 && i<(int)cadena.size()-1) {
							if (lang[LS_LAZY_SYNTAX] && cadena[i]==' ') cadena[i]=',';
							if (cadena[i]==' ' && cadena[i-1]!='&' && cadena[i-1]!='|'  && cadena[i+1]!='&'  && cadena[i+1]!='|')
								err_handler.SyntaxError(54,"Se esperaba fin de expresión.");
						}
						if (parentesis==0 && cadena[i]==',') { // comprobar validez
							std::string str=cadena.substr(last_i,i-last_i);
							if (this_instruction_is_ok()) EvaluarSC(rt,str);
							last_i=i+1;
							inst_impl.expresiones.push_back(str);
						}
					}
					cadena[cadena.size()-1]=';';
				}
			}
			if (inst.type==IT_ESPERAR){  // ------------ ESPERAR -----------//
				if (cadena=="" || cadena==";") err_handler.SyntaxError(217,"Faltan parámetros.");
				else {
					auto &inst_impl = getImpl<IT_ESPERAR>(inst);
					std::string &tiempo = inst_impl.tiempo = cadena;
					if (LastCharIs(tiempo,';')) EraseLastChar(tiempo);
					int &factor = inst_impl.factor;
					if      (RightCompare(tiempo,lang.keywords[KW_SEGUNDOS],true)) factor = 1000;
					else if (RightCompare(tiempo,lang.keywords[KW_MILISEGUNDOS],true)) factor = 1;
					else if (!ignore_logic_errors) err_handler.SyntaxError(218,"Falta unidad o unidad desconocida.");
					DataValue res = EvaluarSC(rt,tiempo,vt_numerica);
					if (!res.CanBeReal()) err_handler.SyntaxError(219,"La longitud del intervalo debe ser numérica."); 
					else {
						for (int tmp1=0;tmp1<(int)tiempo.size();tmp1++) if (tiempo[tmp1]==' ') err_handler.SyntaxError(240,"Se esperaba una sola expresión.");
					}
				}
			}
			if (inst.type==IT_DIMENSION){  // ------------ DIMENSION/REDIMENSION -----------//
				if (cadena=="" || cadena==";") err_handler.SyntaxError(56,"Faltan parámetros.");
				else {
					auto &inst_impl = getImpl<IT_DIMENSION>(inst);
					if (LastCharIs(cadena,';')) EraseLastChar(cadena);
					auto arrays = splitArgsList(cadena);
					for(const auto &array_dec : arrays) {
						auto pos_par = array_dec.find('(',0);
						if (pos_par==std::string::npos) { 
							if (!ignore_logic_errors) err_handler.SyntaxError(58,"Faltan subindices.");
							if (CheckVariable(rt,array_dec,59) and (not memoria->EstaDefinida(array_dec)))
								memoria->DefinirTipo(array_dec,vt_desconocido); // para que aparezca en la lista de variables
						} else {
							// check name
							std::string aname = array_dec.substr(0,pos_par);
							CheckVariable(rt,aname,60);
							if (memoria->EsArgumento(aname) && !ignore_logic_errors) err_handler.SyntaxError(223,"No debe redimensionar un argumento.");
							
							// complete inst_impl
							int close_par = matchParentesis(array_dec,pos_par);
							if (close_par+1!=array_dec.size())
								if (!ignore_logic_errors) err_handler.SyntaxError(57,MkErrorMsg("Se esperaba coma o fin de la instrucción luego de las dimensiones del arreglo $.",aname));
							std::string str_dims = array_dec.substr(pos_par+1,close_par-pos_par-1);
							inst_impl.nombres.push_back(aname);
							inst_impl.tamanios.push_back(str_dims);
							
							// analize dimensions values
							auto vstr_dims = splitArgsList(str_dims);
							int *dims = new int[vstr_dims.size()+1], idim=0; dims[0] = vstr_dims.size();
							DataValue res;
							for (const std::string &str_dim : vstr_dims) {
								dims[++idim] = -1;
								if (ignore_logic_errors) continue;
								if (str_dim=="") err_handler.SyntaxError(61,"Parametro nulo.");
								/*if (this_instruction_is_ok()) */res = EvaluarSC(rt,str_dim,vt_numerica);
								if (res.IsOk() and (not res.CanBeReal())) {
									err_handler.SyntaxError(62,MkErrorMsg("No coinciden los tipos; en la expresion $",str_dim));
								} else {
									if (IsNumericConstant(str_dim)) {
										if ((not lang[LS_ALLOW_RESIZE_ARRAYS]) and str_dim=="0")
											err_handler.SyntaxError(274,MkErrorMsg("Las dimensiones no pueden ser 0, en: $",array_dec));
										else if ((not lang[LS_ALLOW_RESIZE_ARRAYS]) and str_dim=="1")
											err_handler.CompileTimeWarning(333,MkErrorMsg("El tamaño 1 para una dimensión probablemente sea un error, en: $",array_dec));
										else if (res.GetAsInt()<0)
											err_handler.SyntaxError(274,MkErrorMsg("Las dimensiones no pueden ser negativas, en: $",array_dec));
										else if (not IsInteger(res.GetAsReal()))
											err_handler.SyntaxError(331,MkErrorMsg("Las dimensiones deben ser números enteros, en: $",array_dec));
										else 
											dims[idim]=res.GetAsInt();
									} else {
										if (not lang[LS_ALLOW_DINAMYC_DIMENSIONS])
											err_handler.SyntaxError(153,MkErrorMsg("Las dimensiones deben ser constantes, en: $",array_dec));
									}
								}
							}
							
							// store in memory
							if (aname.size()) memoria->AgregarArreglo(aname,dims);
						}
					}
				}
				if (LastCharIs(cadena,';')) cadena[cadena.size()-1]=';';
			}
			if (inst.type==IT_LEER){  // ------------ LEER -----------//
				auto &inst_impl = getImpl<IT_LEER>(inst);
				if (cadena=="" || cadena==";") err_handler.SyntaxError(63,"Faltan parámetros.");
				else {
					if (LastCharIs(cadena,';')) EraseLastChar(cadena);
					auto args_v = splitArgsList(cadena);
					for(auto &var_name : args_v) {
						inst_impl.variables.push_back(var_name);
						auto pos_par = var_name.find("(",0);
						if (pos_par==std::string::npos) {
							if (CheckVariable(rt,var_name,65)) {
								if (!memoria->EstaDefinida(var_name)) memoria->DefinirTipo(var_name,vt_desconocido); // para que aparezca en la lista de variables
								if (memoria->LeerDims(var_name) && !ignore_logic_errors) err_handler.SyntaxError(255,MkErrorMsg("Faltan subindices para el arreglo ($).",var_name));
							}
						} else if (!memoria->EsArgumento(var_name.substr(0,var_name.find('(',0)))) {
							bool name_ok=true;
							std::string aname = var_name.substr(0,pos_par);
							if (!CheckVariable(rt,aname,66)) { name_ok=false; }
							else if (!memoria->EstaDefinida(aname)) memoria->DefinirTipo(aname,vt_desconocido); // para que aparezca en la lista de variables
							if (!memoria->LeerDims(aname) && !ignore_logic_errors) { 
								err_handler.SyntaxError(256,MkErrorMsg("La variable ($) no es un arreglo.",aname));
							} else if (matchParentesis(var_name,pos_par)!=var_name.size()-1) {
								err_handler.SyntaxError(330,MkErrorMsg("Identificador o lista de índices incorrecta ($)",var_name));
							} else {
								std::string s_indexes = var_name.substr(pos_par+1,var_name.size()-pos_par-2);
								auto v_indexes = splitArgsList(s_indexes);
								for(const auto &one_index : v_indexes) {
									// if (str2=="") err_handler.SyntaxError(67,"Parametro nulo.");
									DataValue res;
									if (this_instruction_is_ok()) res = EvaluarSC(rt,one_index,vt_numerica);
									if (res.IsOk()&&!res.CanBeReal())
										err_handler.SyntaxError(154,"No coinciden los tipos.");
								}
								if (name_ok && memoria->LeerDims(aname)[0]!=int(v_indexes.size()) && !ignore_logic_errors) {
									err_handler.SyntaxError(257,MkErrorMsg("Cantidad de indices incorrecta para el arreglo ($).",aname));
									return;
								}
							}
						}
					}
				}
			}
			if (inst.type==IT_PARA) {  // ------------ PARA -----------//
				auto &inst_impl = getImpl<IT_PARA>(inst);
				
				// verificar "hacer"
				if (not RightCompare(cadena,lang.keywords[KW_HACER],true)) {
					if (not lang[LS_LAZY_SYNTAX]) err_handler.SyntaxError(71,MkErrorMsg("Falta $.",kw2str(KW_HACER)));
				}
				
				// cortar hasta
				int pos_hasta = FindKeyword(cadena,lang.keywords[KW_HASTA],true), pos_paso = -1;
				if (pos_hasta==-1) {
					err_handler.SyntaxError(78,MkErrorMsg("Falta $.",kw2str(KW_HASTA)));
				} else {
					std::string asignacion = cadena.substr(0,pos_hasta);
					inst_impl.val_fin = cadena.substr(pos_hasta);
					
					// cortar paso
					pos_paso = FindKeyword(inst_impl.val_fin,lang.keywords[KW_CONPASO],true);
					if (pos_paso!=-1) {
						inst_impl.paso = inst_impl.val_fin.substr(pos_paso);
						inst_impl.val_fin.erase(pos_paso);
					} else {
						pos_paso = FindKeyword(asignacion,lang.keywords[KW_CONPASO],true);
						if (pos_paso!=-1) {
							if (not lang[LS_LAZY_SYNTAX]) err_handler.SyntaxError(216,MkErrorMsg("$ va despues de $.",kw2str(KW_CONPASO),kw2str(KW_HASTA)));
							inst_impl.paso = asignacion.substr(pos_paso);
							asignacion.erase(pos_paso);
						}
					}
					
					// validar contador y valor inicial
					size_t pos_flecha = asignacion.find("<-",0); /// @todo: contemplar los otros operadores de asignacion alternativos (:= e =)
					int pos_corte = -1;
					if (pos_flecha==std::string::npos) {
						if (lang[LS_LAZY_SYNTAX]) pos_corte = FindKeyword(asignacion,lang.keywords[KW_DESDE],true);
						if (pos_corte==-1) err_handler.SyntaxError(72,"Se esperaba asignación.");
					} else {
						pos_corte = pos_flecha;
						asignacion.erase(pos_flecha,2);
					}
					if (pos_corte!=-1) {
						inst_impl.contador = asignacion.substr(0,pos_corte);
						inst_impl.val_ini = asignacion.substr(pos_corte);
						if (pos_flecha!=-1) // antes se encerró el valor entre parentesis para evitar generar un error por -- si el val iniciar es negativo (x<--1)
							inst_impl.val_ini = inst_impl.val_ini.substr(1,inst_impl.val_ini.size()-2);
						// contador
						if (inst_impl.contador.empty() or inst_impl.val_ini.empty())
							err_handler.SyntaxError(73,"Asignacion incompleta.");
						else if (CheckVariable(rt,inst_impl.contador,74)) {
							memoria->DefinirTipo(inst_impl.contador,vt_numerica); // para que aparezca en la lista de variables
							DataValue res;
							if (this_instruction_is_ok()) DataValue res = EvaluarSC(rt,inst_impl.contador,vt_numerica);
							if (res.IsOk() and (not res.CanBeReal())) err_handler.SyntaxError(76,"No coinciden los tipos.");
						}
						// valor inicial
						if (this_instruction_is_ok()) {
							DataValue res = EvaluarSC(rt,inst_impl.val_ini,vt_numerica,"valor inicial");
							if (res.IsOk() and (not res.CanBeReal())) err_handler.SyntaxError(77,"No coinciden los tipos.");
						}
					}
					
					// validar valor final
					if (inst_impl.val_fin.empty()) 
						err_handler.SyntaxError(79,MkErrorMsg("Falta el valor final del $.",kw2str(KW_PARA)));
					else {
						DataValue res;
						if (this_instruction_is_ok()) res = EvaluarSC(rt,inst_impl.val_fin,vt_numerica,"valor final");
						if (res.IsOk()&&!res.CanBeReal()) err_handler.SyntaxError(81,"No coinciden los tipos.");
					}
						
					// validar paso
					if (pos_paso!=-1) {
						if (inst_impl.paso.empty())
							err_handler.SyntaxError(258,"Falta el valor del paso.");
						else {
							DataValue res = EvaluarSC(rt,inst_impl.paso,vt_numerica,"paso");
							if (res.IsOk() and (not res.CanBeReal())) err_handler.SyntaxError(84,"No coinciden los tipos.");
						}
					}
				}
			}
			
			if (inst.type==IT_PARACADA){  // ------------ PARA CADA -----------//
				auto &inst_impl = getImpl<IT_PARACADA>(inst);
				
				// verificar "hacer"
				if (not RightCompare(cadena,lang.keywords[KW_HACER],true)) {
					if (not lang[LS_LAZY_SYNTAX]) err_handler.SyntaxError(71,MkErrorMsg("Falta $.",kw2str(KW_HACER)));
				}
				
				// cortar hasta
				int pos_de = FindKeyword(cadena,lang.keywords[KW_DE],true), pos_paso = -1;
				if (pos_de==-1) {
					err_handler.SyntaxError(260,MkErrorMsg("Falta $.",kw2str(KW_DE)));
				} else {
					inst_impl.identificador = cadena.substr(0,pos_de);
					inst_impl.arreglo = cadena.substr(pos_de);
					if (inst_impl.identificador.empty() or inst_impl.arreglo.empty())
						err_handler.SyntaxError(260,"Falta el identificador de elemento y/o el del arreglo.");
					if (not inst_impl.identificador.empty())
						CheckVariable(rt,inst_impl.identificador,259);
					if (not inst_impl.arreglo.empty())
						CheckVariable(rt,inst_impl.arreglo,261);
				}
			}
			
			if (inst.type==IT_DEOTROMODO) {  // ------------ opcion del SEGUN -----------//
				if ((!bucles.empty())and(programa[bucles.back()]==IT_SEGUN))
					getImpl<IT_SEGUN>(programa[bucles.back()]).opciones.push_back(prog_idx);
			}
			if (inst.type==IT_OPCION) {  // ------------ opcion del SEGUN -----------//
				if (programa[bucles.back()]!=IT_SEGUN) 
					err_handler.SyntaxError(241,MkErrorMsg("Opción fuera de $.",kw2str(KW_SEGUN)));
				
				auto &inst_impl = getImpl<IT_OPCION>(inst);
				if ((!bucles.empty())and(programa[bucles.back()]==IT_SEGUN))
					getImpl<IT_SEGUN>(programa[bucles.back()]).opciones.push_back(prog_idx);
				// permitir utiliza O para separar la posibles opciones
				if (lang[LS_LAZY_SYNTAX]) {
					size_t p;
					while ((p=cadena.find(" O "))!=std::string::npos) cadena.replace(p,3,",");
					while ((p=cadena.find("|")  )!=std::string::npos) cadena.replace(p,1,",");
				}
				cadena[cadena.size()-1]=',';
				int i=0, p;
				while ((p=PSeudoFind(cadena,',',i))!=-1) {
					inst_impl.expresiones.push_back(cadena.substr(i,p-i));
					DataValue res = EvaluarSC(rt,inst_impl.expresiones.back(),
											  lang[LS_INTEGER_ONLY_SWITCH]?vt_numerica:vt_caracter_o_numerica);
					if (res.IsOk() && !res.CanBeReal()&&lang[LS_INTEGER_ONLY_SWITCH]) {
						err_handler.SyntaxError(203,"Las opciones deben ser de tipo numérico.");
					}
					i=p+1;
				}
				cadena[cadena.size()-1]=':';
			}
			
			if (inst.type==IT_ASIGNAR) {  // ------------ ASIGNACION -----------//
				auto &inst_impl = getImpl<IT_ASIGNAR>(inst);
				auto pos_arrow = cadena.find("<-",0);
				std::string &var = inst_impl.variable = cadena.substr(0,pos_arrow);
				if (var.empty())
					err_handler.SyntaxError(85,"Asignación incompleta.");
				else {
					CheckVariable(rt,var,86);
					std::string str = cadena.substr(pos_arrow+2);
					bool comillas=false; int parentesis=0;
					for (int y=0;y<(int)str.size();y++){ // comprobar que se un solo parametro
						if (str[y]=='(') parentesis++;
						if (str[y]==')') parentesis--;
						if (str[y]=='\'') comillas=!comillas;
						if (y>0 && y<(int)str.size()-1)
							if ((!comillas) && str[y]==' ' && str[y-1]!='&' && str[y-1]!='|'  && str[y+1]!='&'  && str[y+1]!='|')
								err_handler.SyntaxError(87,"Se esperaba fin de expresión.");
						if ((!comillas) && parentesis==1 && str[y]==',')
							err_handler.SyntaxError(88,"Demasiados parámetros.");
					}
					if (str.size()==3) // "();"
						err_handler.SyntaxError(89,"Asignación incompleta.");
					else {
						str.erase(0,1); str.erase(str.size()-2,2); // eliminar parentesis y punto y comma
						inst_impl.valor = str;
						tipo_var tipo_left = memoria->LeerTipo(inst_impl.variable);
						tipo_left.rounded = false; // no transferir a la expresión
						DataValue res;
						if (this_instruction_is_ok()) res = EvaluarSC(rt,str,tipo_left.is_ok()?tipo_left:vt_desconocido);
						if (res.IsOk()&&!res.type.can_be(tipo_left)) {
							err_handler.SyntaxError(125,"No coinciden los tipos.");
							if (!memoria->EstaDefinida(str)) memoria->DefinirTipo(str,vt_desconocido); // para que aparezca en la lista de variables
						} else {
							res.type.rounded = false; // no forzar a entero la variable asignada
							memoria->DefinirTipo(inst_impl.variable,res.type);
						}
					}
				}
			}
			if (inst.type==IT_SI){  // ------------ SI -----------//
				if (cadena=="")
					err_handler.SyntaxError(90,MkErrorMsg("Falta la condición en la estructura $-$",kw2str(KW_SI),kw2str(KW_ENTONCES)));
				// comprobar que no halla espacios
				bool comillas=false;
				for (int tmp1=0;tmp1<(int)cadena.size();tmp1++) {
					if (cadena[tmp1]=='\'') comillas=!comillas;
					if (tmp1>0 && tmp1<(int)cadena.size()-1) {
						// si encuentra un espacio (que no saco Operadores) es porque habia una instrucción despues del si, faltaba el "ENTONCES"
						if ((!comillas) && cadena[tmp1]==' ' && cadena[tmp1-1]!='&' && cadena[tmp1-1]!='|'  && cadena[tmp1+1]!='&'  && cadena[tmp1+1]!='|') {
							if (lang[LS_LAZY_SYNTAX]) {
								programa.Insert(prog_idx+1,cadena.substr(tmp1));
								cadena.erase(tmp1);
								break;
							} else {
								err_handler.SyntaxError(91,MkErrorMsg("Se esperaba $ o fin de expresión.",kw2str(KW_ENTONCES)));
							}
						}
					}
				}
				DataValue res;
				if (this_instruction_is_ok()) res = EvaluarSC(rt,cadena,vt_logica);
				if (res.IsOk()&&!res.CanBeLogic()) err_handler.SyntaxError(92,"No coinciden los tipos.");
				getImpl<IT_SI>(inst).condicion = cadena;
			}
			if (inst.type==IT_HASTAQUE){  // ------------ HASTA QUE -----------//
				if (cadena==""||cadena==";") { // cual era la segunda cadena??? (decir cadena==""||cadena=="", puse el ; por instinto)
					 err_handler.SyntaxError(93,MkErrorMsg("Falta la condición en la estructura $.",kw2str(KW_REPETIR))); 
					 cadena+=" ";
				} else {
					std::string str=cadena; // Comprobar la condición
					// comprobar que no halla espacios
					bool comillas=false;
					for (int tmp1=0;tmp1<(int)str.size();tmp1++) {
						if (str[tmp1]=='\'') comillas=!comillas;
						if (tmp1>0 && tmp1<(int)str.size()-1)
							if ((!comillas) && str[tmp1]==' ' && str[tmp1-1]!='&' && str[tmp1-1]!='|'  && str[tmp1+1]!='&'  && str[tmp1+1]!='|')
								err_handler.SyntaxError(94,"Se esperaba fin de expresión.");
					}
					if (LastCharIs(str,';')) {
						str=str.substr(0,str.size()-1);
						cadena=cadena.substr(0,cadena.size()-1);
					}
					DataValue res;
					if (this_instruction_is_ok()) res = EvaluarSC(rt,str,vt_logica);
					if (res.IsOk()&&!res.CanBeLogic()) err_handler.SyntaxError(95,"No coinciden los tipos.");
					getImpl<IT_HASTAQUE>(inst).condicion = str;
				}
			}
			if (inst.type==IT_SEGUN){  // ------------ SEGUN -----------//
				auto &inst_impl = getImpl<IT_SEGUN>(inst);
				
				if (LastCharIs(cadena,';')) {
					if (!ignore_logic_errors) err_handler.SyntaxError(323,MkErrorMsg("$ no lleva punto y coma luego de la expresión de control.",kw2str(KW_MIENTRAS)));
					cadena.erase(cadena.size()-1,1);
				}
				if (not RightCompare(cadena,lang.keywords[KW_HACER],true)) {
					if (not lang[LS_LAZY_SYNTAX]) err_handler.SyntaxError(97,MkErrorMsg("Falta $.",kw2str(KW_HACER)));
				}
				
				if (cadena.empty()) 
					err_handler.SyntaxError(96,MkErrorMsg("Falta la variable/expresión de control en la estructura $.",kw2str(KW_SEGUN)));
				else {
					inst_impl.expresion = cadena; // Comprobar la condición
					DataValue res = EvaluarSC(rt,inst_impl.expresion,lang[LS_INTEGER_ONLY_SWITCH]?vt_numerica:vt_caracter_o_numerica,"expresión de control");
					if (res.IsOk() and (not res.CanBeReal()) and lang[LS_INTEGER_ONLY_SWITCH]) err_handler.SyntaxError(100,"No coinciden los tipos.");
				}
			}
			if (inst.type==IT_MIENTRAS) { // ------------ MIENTRAS -----------//
				auto &inst_impl = getImpl<IT_MIENTRAS>(inst);
				
				if (LastCharIs(cadena,';')) {
					if (!ignore_logic_errors) err_handler.SyntaxError(262,MkErrorMsg("$ no lleva punto y coma luego de la condición.",kw2str(KW_MIENTRAS)));
					cadena.erase(cadena.size()-1,1);
				}
				if (not RightCompare(cadena,lang.keywords[KW_HACER],true)) {
					if (not lang[LS_LAZY_SYNTAX]) err_handler.SyntaxError(102,MkErrorMsg("Falta $.",kw2str(KW_HACER)));
				}
				if (cadena.empty()) 
					err_handler.SyntaxError(101,MkErrorMsg("Falta la condición en la estructura $.",kw2str(KW_MIENTRAS)));
				else {
					// Comprobar la condición
					inst_impl.condicion = cadena; 
					DataValue res = EvaluarSC(rt,inst_impl.condicion,vt_logica,"condición");
					if (res.IsOk() and (not res.CanBeLogic())) err_handler.SyntaxError(104,"No coinciden los tipos.");
				}
			}
			if (inst.type==IT_SINO) {
				if (bucles.empty() || programa[bucles.back()]!=IT_SI) err_handler.SyntaxError(2,MkErrorMsg("$ mal colocado.",kw2str(KW_SINO)));
				else {
					auto &si_impl = getImpl<IT_SI>(programa[bucles.back()]);
					if (si_impl.sino!=-1) err_handler.SyntaxError(322,MkErrorMsg("No puede haber más de un $.",kw2str(KW_SINO)));
					else si_impl.sino = prog_idx;	
				}
			}
			bool fin_algo = 
				inst.type==IT_FINSI || inst.type==IT_FINPARA || inst.type==IT_FINMIENTRAS ||
				inst.type==IT_FINPROCESO  || inst.type==IT_FINSEGUN;
			if (fin_algo || inst.type==IT_REPETIR || inst.type==IT_BORRARPANTALLA || inst.type==IT_ESPERARTECLA)
			{
				if (fin_algo && cadena==";") cadena="";
				else if (!cadena.empty() && cadena!=";") {
					err_handler.SyntaxError(105,"La instrucción no debe tener parámetros.");
					cadena="";
				}
			}
			if (inst.type==IT_ERROR && cadena!="" && cadena!=";") {
				if (LeftCompare(cadena,"FIN "))
					err_handler.SyntaxError(99,"Instrucción no válida.");
				else
					err_handler.SyntaxError(106,"Instrucción no válida.");
			}
			// llama directa a un subproceso
			if (inst.type==IT_INVOCAR) {
				auto &inst_impl = getImpl<IT_INVOCAR>(inst);
				int p=0;
				const std::string &fname = inst_impl.nombre = NextToken(cadena,p);
				const Funcion *func = rt.funcs.GetFunction(fname);
				std::string &args = inst_impl.args = cadena.substr(p);
				if (func->GetTipo(0)!=vt_error && !ignore_logic_errors) err_handler.SyntaxError(310,MkErrorMsg("La función retorna un valor, debe ser parte de una expresión ($).",fname));
				if (args==";") args="();"; // para que siempre aparezcan las llaves y se eviten así problemas
				if (args=="();") {
					if (func->GetArgsCount()!=0 && !ignore_logic_errors) err_handler.SyntaxError(264,MkErrorMsg("Se esperaban argumentos para el $ ($).",kw2str(KW_SUBALGORITMO),fname));
				} else if (func->GetArgsCount()==0) {
					if (args!="();" && !ignore_logic_errors) err_handler.SyntaxError(265,MkErrorMsg("El $ ($) no debe recibir argumentos.",kw2str(KW_SUBALGORITMO),fname));
				} else if (args[0]!='(' && !ignore_logic_errors) err_handler.SyntaxError(266,MkErrorMsg("Los argumentos para invocar a un $ deben ir entre paréntesis.",kw2str(KW_SUBALGORITMO)));
				else { // entonces tiene argumentos, y requiere argumentos, ver que la cantidad esté bien
					int args_last_pos = BuscarComa(args,1,args.length()-1,')');
					if (args_last_pos!=-1) { // si faltaban cerrar parentesis, el error salto antes
						int pos_coma=0, last_pos_coma=0, cant_args=0;
						do {
							pos_coma=BuscarComa(args,pos_coma+1,args_last_pos,',');
							if (pos_coma==-1) pos_coma=args_last_pos;
							if (cant_args<func->GetArgsCount()) {
								std::string arg_actual=args.substr(last_pos_coma+1,pos_coma-last_pos_coma-1);
								if (not SirveParaReferencia(rt,arg_actual)) { // puede ser el nombre de un arreglo suelto, para pasar por ref, y el evaluar diria que faltan los subindices
									if (func->pasajes[cant_args+1]==PP_REFERENCIA && !ignore_logic_errors) err_handler.SyntaxError(268,MkErrorMsg("No puede utilizar una expresión en un pasaje por referencia ($).",arg_actual));
									else EvaluarSC(rt,arg_actual,func->tipos[cant_args+1]);
								}
							}
							cant_args++; last_pos_coma=pos_coma;
						} while (pos_coma!=args_last_pos);
						if (cant_args!=func->GetArgsCount() && !ignore_logic_errors) 
							err_handler.SyntaxError(267,MkErrorMsg("Cantidad de argumentos incorrecta para el subproceso ($).",fname));
						else if (args_last_pos!=int(args.length())-2) err_handler.SyntaxError(269,"Se esperaba fin de instrucción."); // el -2 de la condición es por el punto y coma
					}
				}
				args.erase(args.size()-1);
			}
			// Controlar Cierre de Bucles
			if (inst.type==IT_FINSEGUN) {
				if (!bucles.empty() && programa[bucles.back()]==IT_SEGUN) {
					getImpl<IT_SEGUN>(programa[bucles.back()]).fin = prog_idx;	
					bucles.pop_back();
				} else
					err_handler.SyntaxError(107,MkErrorMsg("$ mal colocado.",kw2str(KW_FINSEGUN)));
			}
			if (inst.type==IT_FINPARA) {
				if (!bucles.empty() && (programa[bucles.back()]==IT_PARA||programa[bucles.back()]==IT_PARACADA)) {
					if  (programa[bucles.back()]==IT_PARA)
						getImpl<IT_PARA>(programa[bucles.back()]).fin = prog_idx;	
					else 
						getImpl<IT_PARACADA>(programa[bucles.back()]).fin = prog_idx;	
					bucles.pop_back();
				} else
					err_handler.SyntaxError(108,MkErrorMsg("$ mal colocado.",kw2str(KW_FINPARA)));
			}
			if (inst.type==IT_FINMIENTRAS) {
				if (!bucles.empty() && (programa[bucles.back()]==IT_MIENTRAS)) {
					getImpl<IT_MIENTRAS>(programa[bucles.back()]).fin = prog_idx;	
					bucles.pop_back();
				} else
					err_handler.SyntaxError(109,MkErrorMsg("$ mal colocado.",kw2str(KW_FINMIENTRAS)));
			}
			if (inst.type==IT_FINSI) {
				if (!bucles.empty() && (programa[bucles.back()]==IT_SI)) {
					getImpl<IT_SI>(programa[bucles.back()]).fin = prog_idx;	
					bucles.pop_back();
				} else
					err_handler.SyntaxError(110,MkErrorMsg("$ mal colocado.",kw2str(KW_FINSI)));
			}
			if (inst.type==IT_HASTAQUE) {
				if (!bucles.empty() && programa[bucles.back()]==IT_REPETIR) {
					getImpl<IT_REPETIR>(programa[bucles.back()]).fin = prog_idx;	
					bucles.pop_back();
				} else {
					if (getImpl<IT_HASTAQUE>(inst).mientras_que)
						err_handler.SyntaxError(270,MkErrorMsg("$ mal colocado.",kw2str(KW_MIENTRASQUE))); 
					else
						err_handler.SyntaxError(111,MkErrorMsg("$ mal colocado.",kw2str(KW_HASTAQUE))); 
				}
			}
			if ( (prog_idx>0 && inst.type==IT_SINO && programa[prog_idx-1]==IT_SI)
				|| (prog_idx>0 && inst.type==IT_SINO && programa[prog_idx-1]==IT_ENTONCES) )
			{
				if (!ignore_logic_errors) err_handler.SyntaxError(113,"Debe haber acciones en la salida por verdadero.");
			}
			
			inst.type = inst.type;
			if ((inst.type==IT_NULL||inst.type==IT_ERROR) && (cadena.size()==0 || cadena==";")) {
				programa.Erase(prog_idx);prog_idx--;
			} // Borra cadenas vacias
			
		}
	}
	
	InformUnclosedLoops(rt,bucles);
	
}

bool ParseInspection(RunTime &rt, std::string &cadena) {
	auto ret = Normalizar(cadena); // acomodar caracteres
	if (!ret.first.empty()) rt.err.SyntaxError(271,"No puede haber más de una expresión ni comentarios.");
	if (LastCharIs(cadena,';')) EraseLastChar(cadena);
	Condiciones(rt,cadena); // word_operators
	Operadores(rt,-1,cadena,IT_ASIGNAR); // verificar operadores
	return rt.err.IsOk();
}

static void parseEasterEggInComment(RunTime &rt, std::string &str, CodeLocation loc) {
	auto p_egg = str.find("PSeInt apesta");
	if (p_egg==std::string::npos) return;
	str.insert(p_egg+6," no");
	Inter.SetLocation(loc);
	rt.err.CompileTimeWarning(332,"Hieres mis sentimientos :(");
	Inter.SetLocation({});
}

bool SynCheck(RunTime &rt) {
	Programa &programa = rt.prog;
	ErrorHandler &err_handler = rt.err;
	auto kw2str = [&keys=lang.keywords](int i) { return keys[i].get(); };
	
	programa.PushBack(""); // linea en blanco al final, para que era?
	programa.Insert(0,""); // linea en blanco al principio, para que era?
	
	if (case_map) for(int i=0;i<programa.GetInstCount();i++) CaseMapFill(rt,programa[i].instruccion);
	
	// pasar todo a mayusculas, reemplazar tabs, comillas, word_operators, corchetes, y trimear
	for(int i=0;i<programa.GetInstCount();i++) {
		auto ret = Normalizar(programa[i].instruccion);
		if (ret.first.empty()) continue;
		// si despues de la 1ra instruccion habia otra, o habia un comentario
		if (ret.second) { // comentario...
			parseEasterEggInComment(rt,ret.first,programa[i].loc);
			if (preserve_comments and (not ret.first.empty())) {
				if (not programa[i].instruccion.empty())
					programa.Insert(++i,"//");
				programa[i].setType(IT_COMMENT);
				getImpl<IT_COMMENT>(programa[i]).text = ret.first;
			}
		} else { // 2da instruccion
			programa.Insert(i+1,ret.first);
		}
	}
	
	
	_expects(not rt.funcs.HaveMain());
	for(int i=0;i<programa.GetInstCount();i++) {
		std::string &src = programa[i].instruccion;
		bool es_proceso = LeftCompare(src,lang.keywords[KW_ALGORITMO],false);
		if (es_proceso or LeftCompare(src,lang.keywords[KW_SUBALGORITMO],false)) {
			Inter.SetLocation(programa[i].loc);
			std::string proto = src; LeftCompare(proto,lang.keywords[es_proceso?KW_ALGORITMO:KW_SUBALGORITMO],true);
			auto func = MakeFuncionForSubproceso(rt,proto,es_proceso);
			func->userline_start = Inter.GetLocation().linea;
			std::string func_name = func->id;
			rt.funcs.AddSub(std::move(func));
			if (es_proceso) { // si es el proceso principal, verificar que sea el unico, y guardar el nombre en main_process_name para despues saber a cual llamar
				if (rt.funcs.HaveMain())
					err_handler.SyntaxError(272,MkErrorMsg("Solo puede haber un $.",kw2str(KW_ALGORITMO)));
				rt.funcs.SetMain(func_name);
			} else if (!lang[LS_ENABLE_USER_FUNCTIONS])
				err_handler.SyntaxError(309,MkErrorMsg("Este perfil no admite $.",kw2str(KW_SUBALGORITMO)));
		}
	}
	Instrucciones(rt);
	
	if (not rt.funcs.HaveMain()) { Inter.SetLocation({1,1}); err_handler.SyntaxError(273,MkErrorMsg("Debe haber un $.",kw2str(KW_ALGORITMO))); }
	else Inter.SetLocation({rt.funcs.GetMainFunc()->line_start,1});
	
	return err_handler.IsOk();
}
