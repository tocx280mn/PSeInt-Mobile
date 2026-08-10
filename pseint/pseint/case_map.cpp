#include "case_map.h"
#include "utils.h"
#include "FuncsManager.hpp"
#include "RunTime.hpp"
#include "Programa.hpp"
#include "strFuncs.hpp"

std::map<std::string,std::string> *case_map = nullptr;

//----------------from wxPSeInt/CommonParsinsFunctions------
// tengo que unificar las mil implementaciones de estos auxiliares
template<typename TString> 
int SkipString(const TString &line, int i, int len) {
	do {
		++i;
	} while(i<len && line[i]!='\'' && line[i]=='\"');
	return i;
}

template<typename TString> 
int SkipParentesis(const TString &line, int i, int len) {
	int par_level = 1;
	while (par_level && i<len) {
		++i;
		if (line[i]=='(' || line[i]=='[') ++par_level;
		else if (line[i]==']' || line[i]==')') --par_level;
		else if (line[i]=='\'' || line[i]=='\"') i = SkipString(line,i,len);
	}
	return i;
}
//----------------

static void CaseMapAux(const RunTime &rt, std::string &s, bool fill, bool fix_parentesis) {
	int len=s.size(),p = 0;
	bool comillas=false, word=false;
	for(int i=0;i<=len;i++) { 
		char c = i==len?' ':ToUpper(s[i]);
		if (c=='\''||c=='\"') comillas=!comillas;
		else if (!comillas) {
			if (i&&c=='/'&&s[i-1]=='/') return;
			if (word) {
				if (!EsLetra(c,true)) {
					std::string s1=s.substr(p,i-p);
					unsigned int keylen=s1.size(); 
					if (fill) {
						std::string s2=s1;
						for(unsigned int i=0;i<keylen;i++) 
							s1[i]=ToUpper(s2[i]);
						(*case_map)[s1]=s2;
					} else {
						std::string s2=(*case_map)[s1];
						if (s2.size()==keylen) {
							for(unsigned int j=0;j<keylen;j++) 
								s[p+j]=s2[j];
							if (fix_parentesis && s[i]=='(' && !rt.funcs.IsFunction(s1)) {
								s[i]='['; 
								int i2 = SkipParentesis(s,i,len);
								if (i2<len && s[i2]==')') s[i2]=']';
							}
						}
					}
					word=false;
				}
			} else if (EsLetra(c)) {
				p=i; word=true;
			}
		}
	}
}

void CaseMapFill(const RunTime &rt, std::string &s) {
	CaseMapAux(rt,s,true,false);
}

void CaseMapApply(const RunTime &rt, std::string &s, bool and_fix_parentesis) {
	CaseMapAux(rt,s,false,and_fix_parentesis);
}


//void CaseMapPurge() {
//	
//	(*case_map)["PROCESO"]="";
//	(*case_map)["FINPROCESO"]="";
//	(*case_map)["SUBPROCESO"]="";
//	(*case_map)["FINSUBPROCESO"]="";
//	
//	(*case_map)["SI"]="";
//	(*case_map)["ENTONCES"]="";
//	(*case_map)["SINO"]="";
//	(*case_map)["FINSI"]="";
//	
//	(*case_map)["INVOCAR"]="";
//	(*case_map)["ESCRIBIR"]="";
//	(*case_map)["LEER"]="";
//	(*case_map)["DEFINIR"]="";
//	(*case_map)["DIMENSION"]="";
//	
//	(*case_map)["REPETIR"]="";
//	(*case_map)["HASTA"]="";
//	(*case_map)["QUE"]="";
//	
//	(*case_map)["PARA"]="";
//	(*case_map)["PARACADA"]="";
//	(*case_map)["CON"]="";
//	(*case_map)["PASO"]="";
//	(*case_map)["FINPARA"]="";
//	
//	(*case_map)["MIENTRAS"]="";
//	(*case_map)["FINMIENTRAS"]="";
//	
//	(*case_map)["SEGUN"]="";
//	(*case_map)["DE"]="";
//	(*case_map)["OTRO"]="";
//	(*case_map)["MODO"]="";
//	(*case_map)["FINSEGUN"]="";
//	
//	(*case_map)["DEFINIR"]="";
//	(*case_map)["COMO"]="";
//	(*case_map)["ENTERO"]="";
//	(*case_map)["CARACTER"]="";
//	(*case_map)["REAL"]="";
//	(*case_map)["LOGICO"]="";
//	
////	(*case_map)["LIMPIAR"]="";
////	(*case_map)["BORRAR"]="";
////	(*case_map)["PANTALLA"]="";
////	(*case_map)["ESPERAR"]="";
////	(*case_map)["TECLA"]="";
////	(*case_map)["SEGUNDO"]="";
////	(*case_map)["SEGUNDOS"]="";
////	(*case_map)["MILISEGUNDO"]="";
////	(*case_map)["MILISEGUNDOS"]="";
//	
//}

void InitCaseMap() {
	if (!case_map) case_map = new std::map<std::string,std::string>();
}

#ifndef _FOR_PSEXPORT
void CaseMapApply(const RunTime & rt, Programa &programa, bool for_export) {
	for(Instruccion &inst : programa) {
		switch(inst.type) {
			
		case IT_PROCESO: {
			auto &impl = getImpl<IT_PROCESO>(inst); 
			CaseMapApply(rt,impl.nombre,!for_export);
			CaseMapApply(rt,impl.ret_id,!for_export);
			CaseMapApply(rt,impl.args,!for_export);
		} break;
		
		case IT_FINPROCESO: {
			auto &impl = getImpl<IT_FINPROCESO>(inst); 
			CaseMapApply(rt,impl.nombre,!for_export);
		} break;
		
		case IT_LEER: {
			auto &impl = getImpl<IT_LEER>(inst); 
			for(auto &var : impl.variables)
				CaseMapApply(rt,var,!for_export);
		} break;
		
		case IT_ASIGNAR: {
			auto &impl = getImpl<IT_ASIGNAR>(inst); 
			CaseMapApply(rt,impl.variable,!for_export);
			CaseMapApply(rt,impl.valor,!for_export);
		} break;
		
		case IT_ESCRIBIR: {
			auto &impl = getImpl<IT_ESCRIBIR>(inst);
			for(auto &expr : impl.expresiones)
				CaseMapApply(rt,expr,!for_export);
		} break;
		
		case IT_MIENTRAS: {
			auto &impl = getImpl<IT_MIENTRAS>(inst); 
			CaseMapApply(rt,impl.condicion,!for_export);
		} break;
		
		
		case IT_HASTAQUE: {
			auto &impl = getImpl<IT_HASTAQUE>(inst);
			CaseMapApply(rt,impl.condicion,!for_export);
		} break;
		
		case IT_SI: {
			auto &impl = getImpl<IT_SI>(inst);
			CaseMapApply(rt,impl.condicion,!for_export);
		} break;
		
		case IT_PARA: {
			auto &impl = getImpl<IT_PARA>(inst); 
			CaseMapApply(rt,impl.contador,!for_export);
			CaseMapApply(rt,impl.val_ini,!for_export);
			CaseMapApply(rt,impl.paso,!for_export);
			CaseMapApply(rt,impl.val_fin,!for_export);
		} break;
		
		case IT_PARACADA: {
			auto &impl = getImpl<IT_PARACADA>(inst); 
			CaseMapApply(rt,impl.identificador,!for_export);
			CaseMapApply(rt,impl.arreglo,!for_export);
		} break;
		
		case IT_DIMENSION: {
			auto &impl = getImpl<IT_DIMENSION>(inst);
			for(auto &nom : impl.nombres)
				CaseMapApply(rt,nom,!for_export);
			for(auto &tam : impl.tamanios)
				CaseMapApply(rt,tam,!for_export);
		} break;
		
		case IT_DEFINIR: {
			auto &impl = getImpl<IT_DEFINIR>(inst); 
			for(auto &var : impl.variables)
				CaseMapApply(rt,var,!for_export);
		} break;
		
		case IT_SEGUN: {
			auto &impl = getImpl<IT_SEGUN>(inst);
			CaseMapApply(rt,impl.expresion,!for_export);
		} break;
		
		case IT_OPCION: {
			auto &impl = getImpl<IT_OPCION>(inst);
			for(auto &expr : impl.expresiones)
				CaseMapApply(rt,expr,!for_export);
		} break;
		
		case IT_ESPERAR: {
			auto &impl = getImpl<IT_ESPERAR>(inst); 
			CaseMapApply(rt,impl.tiempo,!for_export);
		} break;
		
		case IT_INVOCAR: {
			auto &impl = getImpl<IT_INVOCAR>(inst); 
			CaseMapApply(rt,impl.nombre,!for_export);
			CaseMapApply(rt,impl.args,!for_export);
		} break;	
		
		case IT_REPETIR:
		case IT_ERROR:
		case IT_ESPERARTECLA:
		case IT_BORRARPANTALLA:
		case IT_FINMIENTRAS:
		case IT_ENTONCES:
		case IT_SINO:
		case IT_FINSI:
		case IT_DEOTROMODO:
		case IT_FINSEGUN:
		case IT_FINPARA:
		case IT_COMMENT:
		case IT_NULL: 
			break;
		}
	}
}
#endif

