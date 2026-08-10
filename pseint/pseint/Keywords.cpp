#include <limits>
#include "Keywords.hpp"
#include "utils.h"
#include "strFuncs.hpp"

/// @todo:  reemplazar por alguna otra func mas específica
std::pair<std::string,bool> Normalizar(std::string &cadena);

void initKeywords(KeywordsList &keywords) {
	keywords[KW_ALGORITMO] = "Algoritmo,Proceso";
	keywords[KW_FINALGORITMO] = "FinAlgoritmo,FinProceso,Fin Algoritmo,Fin Proceso";
	keywords[KW_POR_COPIA] = "Por Copia,Por Valor";
	keywords[KW_POR_REFERENCIA] = "Por Referencia";
	keywords[KW_SUBALGORITMO] = "Función,SubAlgoritmo,SubProceso";
	keywords[KW_FINSUBALGORITMO] = "FinFunción,FinSubAlgoritmo,FinSubProceso,Fin Función,Fin SubAlgoritmo,Fin SubProceso";
	keywords[KW_LEER] = "Leer";
	keywords[KW_ESCRIBIR] = "Escribir,Mostrar,Imprimir,Informar";
	keywords[KW_SIN_SALTAR] = "Sin Saltar,Sin Bajar,SinSaltar,SinBajar";
	keywords[KW_DIMENSIONAR] = "Dimensionar,Dimensión";
	keywords[KW_REDIMENSIONAR] = "Redimensionar,Redimensión";
	keywords[KW_DEFINIR] = "Definir";
	keywords[KW_COMO] = "Como";
	keywords[KW_TIPO_ENTERO] = "Entero,Entera,Enteros,Enteras";
	keywords[KW_TIPO_REAL] = "Real,Reales,Número,Numero,Números,Numérica,Numéricas,Numérico,Numéricos";
	keywords[KW_TIPO_LOGICO] = "Lógico,Lógica,Lógicos,Lógicas";
	keywords[KW_TIPO_CARACTER] = "Cadena,Cadenas,Texto,Textos,Carácter,Caracteres,";
	keywords[KW_ES] = "Es,Son";
	keywords[KW_SI] = "Si";
	keywords[KW_ENTONCES] = "Entonces";
	keywords[KW_SINO] = "SiNo";
	keywords[KW_FINSI] = "FinSi,Fin Si";
	keywords[KW_MIENTRAS] = "Mientras";
	keywords[KW_HACER] = "Hacer";
	keywords[KW_FINMIENTRAS] = "FinMientras,Fin Mientras";
	keywords[KW_REPETIR] = "Repetir";
	keywords[KW_HASTAQUE] = "Hasta Que,HastaQue";
	keywords[KW_MIENTRASQUE] = "Mientras Que,MientrasQue";
	keywords[KW_SEGUN] = "Según";
	keywords[KW_OPCION] = "Si Es,SiEs,Opcion,Caso";
	keywords[KW_DEOTROMODO] = "De Otro Modo,DeOtroModo";
	keywords[KW_FINSEGUN] = "FinSegún,Fin Según";
	keywords[KW_PARA] = "Para";
	keywords[KW_DESDE] = "Desde";
	keywords[KW_HASTA] = "Hasta";
	keywords[KW_CONPASO] = "Con Paso,ConPaso";
	keywords[KW_PARACADA] = "Para Cada,ParaCada";
	keywords[KW_DE] = "De,En";
	keywords[KW_FINPARA] = "FinPara,Fin Para";
	keywords[KW_LIMPIARPANTALLA] = "Limpiar Pantalla,LimpiarPantalla,Borrar Pantalla,BorrarPantalla";
	keywords[KW_ESPERARTECLA] = "Esperar Tecla,EsperarTecla,Esperar Una Tecla";
	keywords[KW_ESPERARTIEMPO] = "Esperar";
	keywords[KW_SEGUNDOS] = "Segundos,Segundo";
	keywords[KW_MILISEGUNDOS] = "Milisegundos,Milisegundo";
	for(const auto &key : keywords) { _expects(key.IsOk()); }
}

Keyword &Keyword::operator=(const std::string &config) {
	clear();
	return operator+=(config);
}

Keyword &Keyword::operator+=(const std::string &config) {
	size_t i = 0, sz = config.size();
	do {
		while(i<sz and (config[i]==' ')) ++i; // skip leading spaces
		int i0 = i;
		while(i<sz and (config[i]!=',')) ++i; // fin ending
		int i1 = i; if (i<sz) ++i;
		while (i1>i0 and config[i1-1]==' ') --i1; // skip trailing spaces
		std::string alt = config.substr(i0,i1-i0);
		if (alternatives.empty()) preferred  = alt;
		if (not alt.empty()) {
			for(char &c : alt) c = Normalize(c);
			alternatives.push_back(alt);
		}
	} while (i<sz);
	return *this;
}

bool StartsWith(const std::string &line, const std::string &start) {
	size_t i = 0, l1 = line.size(), l2 = start.size();
	if (l2>l1) return false;
	for(int i=0; i<l2; ++i) {
		if (line[i]!=start[i])
			return false;
	}
	return true;
}

std::pair<KeywordType,std::string> BestMatch(const KeywordsList &keywords, std::string &src, bool remove) {
	/// @todo: ver si considerar todas las keywords, o poner algun bool que diga cual puede ser comienzo de instruccion
	KeywordType best_match_key = KW_NULL;
	size_t best_match_len = 0;
	auto src_sz = src.size();
	for(size_t i=0;i<keywords.size();++i) {
		if (not keywords[i].IsOk()) continue;
		for(const std::string &alt : keywords[i].alternatives) {
			if (not StartsWith(src,alt)) continue;
			if (src.size()!=alt.size() and (EsLetra(src[alt.size()],true))) continue;
			if (alt.size()<best_match_len) continue;
			best_match_len = alt.size();
			best_match_key = static_cast<KeywordType>(i);
		}
	}
	auto ret = std::make_pair(best_match_key,src.substr(0,best_match_len));
	if (remove) {
		if (best_match_len<src.size() and src[best_match_len]==' ') ++best_match_len;
		src.erase(0,best_match_len);
	}
	return ret;
}

bool RightCompare(std::string &src, const Keyword &keyw, bool remove) {
	for(const std::string &alt : keyw.alternatives) {
		if (not RightCompare(src,alt)) continue;
		if (src.size()!=alt.size() and EsLetra(src[src.size()-alt.size()-1],true)) continue;
		if (remove) {
			src.erase(src.size()-alt.size());
			if (LastCharIs(src,' ')) EraseLastChar(src);
		}
		return true;
	}
	return false;
}

bool LeftCompare(std::string &src, const Keyword &keyw, bool remove) {
	for(const std::string &alt : keyw.alternatives) {
		if (not LeftCompare(src,alt)) continue;
		size_t l = alt.size();
		if (src.size()>l and EsLetra(src[l],true)) continue;
		if (remove) {
			if (src[l]==' ') ++l;
			src.erase(0,l);
		}
		return true;
	}
	return false;
}

int FindKeyword(std::string &src, const Keyword &keyw, bool remove) {
	for(const std::string &alt : keyw.alternatives) {
		for (size_t p = src.find(alt); p!=std::string::npos; p = src.find(p+1)) {
			if (p!=0 and EsLetra(src[p-1],true)) continue;
			if (p+alt.size()<src.size() and EsLetra(src[p+alt.size()],true)) continue;
			bool comillas = false;
			for(int j=0;j<p;++j) 
				if (src[j]=='\'' or src[j]=='\"')
					comillas = not comillas;
			if (comillas) continue;
			if (remove) {
				auto l = alt.size();
				if (p!=0 and src[p-1]==' ') --p, ++l;
				if (p+l<src.size() and src[p+l]==' ') ++l;
				src.erase(p,l);
			}
			return p;
		}
	}
	return -1;
}

static void RemoveAccents(std::string &s) {
	for (char &c : s) {
		switch(c) {
			case 'Á': c = 'A'; break;
			case 'É': c = 'E'; break;
			case 'Í': c = 'I'; break;
			case 'Ó': c = 'O'; break;
			case 'Ú': c = 'U'; break;
			case 'Ü': c = 'U'; break;
			case 'Ñ': c = 'N'; break;
			case 'á': c = 'a'; break;
			case 'é': c = 'e'; break;
			case 'í': c = 'i'; break;
			case 'ó': c = 'o'; break;
			case 'ú': c = 'u'; break;
			case 'ü': c = 'u'; break;
			case 'ñ': c = 'n'; break;
			default: ;
		}
	}
}

void fixKeywords(KeywordsList &keywords, const LangSettings & lang) {
	
	// LS_PREFER_ALGORITMO
	if (not lang[LS_PREFER_ALGORITMO]) {
		keywords[KW_ALGORITMO] = "Proceso, Algoritmo";
		keywords[KW_FINALGORITMO] = "FinProceso, Fin Proceso, FinAlgoritmo, Fin Algoritmo";
	}
	
	// LS_PREFER_FUNCION
	keywords[KW_SUBALGORITMO].clear();
	keywords[KW_FINSUBALGORITMO].clear();
	if (lang[LS_PREFER_FUNCION]) {
		keywords[KW_SUBALGORITMO] += "Función";
		keywords[KW_FINSUBALGORITMO] += "FinFunción, Fin Función";
	}
	if (not lang[LS_PREFER_ALGORITMO]) {
		keywords[KW_SUBALGORITMO] += "SubAlgoritmo, SubProceso";
		keywords[KW_FINSUBALGORITMO] += "FinSubAlgoritmo, Fin SubAlgoritmo, FinSubProceso, Fin SubProceso";
	} else {
		keywords[KW_SUBALGORITMO] += "SubProceso, SubAlgoritmo";
		keywords[KW_FINSUBALGORITMO] += "FinSubProceso, Fin SubProceso, FinSubAlgoritmo, Fin SubAlgoritmo";
	}
	if (not lang[LS_PREFER_FUNCION]) {
		keywords[KW_SUBALGORITMO] += "Función";
		keywords[KW_FINSUBALGORITMO] += "FinFunción, Fin Función";
	}
	
	// LS_ALLOW_ACCENTS
	if (not lang[LS_ALLOW_ACCENTS]) {
		for(auto &key : keywords)
			RemoveAccents(key.preferred);
	}
	
}

