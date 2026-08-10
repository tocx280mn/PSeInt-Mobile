#ifndef COMMONPARSINGFUNCTIONS_H
#define COMMONPARSINGFUNCTIONS_H
#include <wx/string.h>
#include "string_conversions.h"

template<typename TChar>
inline bool EsLetra(const TChar &c, bool incluir_nros=true) {
	return 
		c=='_'||
		((c|32)>='a'&&(c|32)<='z')||
		c=='á'||c=='Á'||c=='é'||c=='É'||
		c=='ó'||c=='Ó'||c=='í'||c=='Í'||
		c=='ú'||c=='Ú'||c=='ñ'||c=='Ñ'||
		(incluir_nros&&c>='0'&&c<='9');
}

template<>
inline bool EsLetra(const wxUniChar &c, bool incluir_nros) {
	static wxString s = _Z("_áéíóúüñÁÉÍÓÚÜÑ");
	return ((_C(c)|32)>='a'&&(_C(c)|32)<='z')||s.Contains(c)||(incluir_nros&&_C(c)>='0'&&_C(c)<='9');
}
template<>
inline bool EsLetra(const wxUniCharRef &c, bool incluir_nros) {
	return EsLetra(wxUniChar(c),incluir_nros);
}

template<typename TChar>
inline bool EsNumero(const TChar &c, bool incluir_punto=true) {
	return (c>='0'&&c<='9') || (incluir_punto&&c=='.');
}

template<typename TChar>
inline bool EsEspacio(const TChar &c) {
	return c==' '||c=='\t';
}

template<typename TChar>
inline bool EsComilla(const TChar &c) {
	return c=='\''||c=='\"';
}


template<typename TString>
int SkipWhite(const TString &s, int i, int l) {
	while (i<l && (s[i]==' '||s[i]=='\t')) i++;
	return i;
}

template<typename TString>
inline int SkipWord(const TString &s, int i, int l) {
	while (i<l && EsLetra(s[i],true)) i++;
	return i;
}

template<typename TString>
void MakeUpper(TString &word) {
	for(unsigned int i=0;i<word.size();i++) { 
		if (word[i]>='a'&&word[i]<='z') word[i] = toupper(word[i]);
		else if (word[i]>='A'&&word[i]<='Z') continue;
		else if (word[i]>='0'&&word[i]<='9') continue;
		else if (word[i]=='á') word[i]='Á';
		else if (word[i]=='é') word[i]='É';
		else if (word[i]=='í') word[i]='Í';
		else if (word[i]=='ó') word[i]='Ó';
		else if (word[i]=='ú') word[i]='Ú';
		else if (word[i]=='ü') word[i]='Ü';
		else if (word[i]=='ñ') word[i]='Ñ';
	}
}

template<>
void MakeUpper(wxString &word) {
	for(unsigned int i=0;i<word.size();i++) {
		auto c = _C(word[i]);
		if (c>='a'&&c<='z') word[i] = toupper(c);
		else if (c>='A'&&c<='Z') continue;
		else if (c>='0'&&c<='9') continue;
		else {
			static wxString sl = _Z("_áéíóúüñ");
			static wxString su = _Z("_ÁÉÍÓÚÜÑ");
			auto p = sl.Index(word[i]);
			if (p!=wxNOT_FOUND) word[i] = su[p];
		}
	}
}

template<typename TString>
TString Upper(TString word) {
	MakeUpper(word);
	return word;
}

template<typename TString> 
int SkipString(const TString &line, int i, int len) {
	do {
		++i;
	} while(i<len && line[i]!='\'' && line[i]!='\"');
	return i;
}

template<typename TString> 
int SkipParentesis(const TString &line, int i, int len) {
	int par_level = 0;
	do {
		if (line[i]=='(' || line[i]=='[') ++par_level;
		else if (line[i]==']' || line[i]==')') --par_level;
		else if (line[i]=='\'' || line[i]=='\"') i = SkipString(line,i,len);
		++i;
	} while(i<len && par_level>0);
	return i;
}

inline wxString GetFirstWord(wxString s) {
	int l = s.Len();
	int i0 = SkipWhite(s,0,l);
	int i1 = SkipWord(s,i0,l);
	if (i1>i0) {
		s = s.Mid(i0,i1-i0);
		MakeUpper(s);
	} else {
		s = "";
	}
	return s;
}

#endif
