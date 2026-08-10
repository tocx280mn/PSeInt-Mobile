#ifndef KEYWORDS_HPP
#define KEYWORDS_HPP

#include <vector>
#include <array>
#include <string>
#include "debug.h"

enum KeywordType {
	KW_ALGORITMO, KW_FINALGORITMO, KW_SUBALGORITMO, KW_FINSUBALGORITMO, KW_POR_COPIA, KW_POR_REFERENCIA,
	KW_LEER, KW_ESCRIBIR, KW_SIN_SALTAR,
	KW_DIMENSIONAR, KW_REDIMENSIONAR, 
	KW_DEFINIR, KW_ES, KW_COMO, KW_TIPO_ENTERO, KW_TIPO_REAL, KW_TIPO_LOGICO, KW_TIPO_CARACTER,
	KW_SI, KW_ENTONCES, KW_SINO, KW_FINSI,
	KW_MIENTRAS, KW_HACER, KW_FINMIENTRAS,
	KW_REPETIR, KW_HASTAQUE, KW_MIENTRASQUE,
	KW_SEGUN, KW_OPCION, KW_DEOTROMODO, KW_FINSEGUN,
	KW_PARA, KW_DESDE, KW_HASTA, KW_CONPASO, KW_PARACADA, KW_DE, KW_FINPARA,
	KW_LIMPIARPANTALLA, KW_ESPERARTECLA, KW_ESPERARTIEMPO, KW_SEGUNDOS, KW_MILISEGUNDOS,
	KW_COUNT
};

constexpr inline KeywordType KW_NULL = KW_COUNT;

struct Keyword {
	Keyword() = default;
	Keyword &operator+= (const std::string &config);
	Keyword &operator= (const std::string &config);
	std::vector<std::string> alternatives; // normalizado
	std::string preferred; // alternatives[0] sin normalizar
	const std::string &get(bool normalized=false) const { return normalized?alternatives[0]:preferred; }
	void clear() { alternatives.clear(); preferred.clear(); }
	bool IsOk() const {
		_expects(alternatives.empty()==preferred.empty());
		return (not preferred.empty());
	}
};

using KeywordsList = std::array<Keyword,KW_COUNT>;

void initKeywords(KeywordsList &keywords);

class LangSettings;
void fixKeywords(KeywordsList &keywords, const LangSettings &lang);

/// Retorna el id de la keyword que encuentra, el texto original copiado de src, 
/// y si remove es true, lo borra de la cadena (incluyendo el espacio que sigue si lo hay)
std::pair<KeywordType,std::string> BestMatch(const KeywordsList &keywords, std::string &src, bool remove);

bool RightCompare(std::string &src, const Keyword &keyw, bool remove);
bool LeftCompare(std::string &src, const Keyword &keyw, bool remove);
	
// retorna la pos donde la encuentra, o -1 si no estaba
int FindKeyword(std::string &src, const Keyword &keyw, bool remove); 


#endif
