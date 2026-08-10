#ifndef LANGSETTINGS_H
#define LANGSETTINGS_H

#include <string>
#include <cstdlib>
#include "Keywords.hpp"
#ifdef FOR_WXPSEINT
#	include <wx/string.h>
#	include "../wxPSeInt/string_conversions.h"
#endif

#define LS_VERSION 20230211

enum LS_INIT_ENUM {
	LS_INIT,
	LS_DO_NOT_INIT
};

enum LS_SOURCE_ENUM {
	LS_DEFAULT,
	LS_FILE,
	LS_LIST,
	LS_CUSTOM
};

enum LS_ENUM {
	LS_FORCE_INIT_VARS,
	LS_FORCE_DEFINE_VARS,
	LS_FORCE_SEMICOLON,
	LS_ALLOW_CONCATENATION,
	LS_ENABLE_STRING_FUNCTIONS,
	LS_WORD_OPERATORS,
	LS_BASE_ZERO_ARRAYS,
	LS_ALLOW_DINAMYC_DIMENSIONS,
	LS_ALLOW_RESIZE_ARRAYS,
	LS_OVERLOAD_EQUAL,
	LS_ENABLE_USER_FUNCTIONS,
	LS_LAZY_SYNTAX,
	LS_COLOQUIAL_CONDITIONS,
	LS_INTEGER_ONLY_SWITCH,
	LS_DEDUCE_NEGATIVE_FOR_STEP,
	LS_USE_NASSI_SHNEIDERMAN,
	LS_USE_ALTERNATIVE_IO_SHAPES,
	LS_ALLOW_ACCENTS,
	LS_PREFER_ALGORITMO,
	LS_PREFER_FUNCION,
	LS_ALLOW_REPEAT_WHILE,
	LS_ALLOW_FOR_EACH,
	LS_PREFER_REPEAT_WHILE,
	LS_PROTECT_FOR_COUNTER,
	LS_COUNT 
};

struct LangSettings {
	
	struct aux_struct { 
		const char *nombre; bool default_value; 
#ifdef FOR_WXPSEINT
		wxString user_desc, long_desc;
#endif
		aux_struct(){ nombre=nullptr; default_value=false; }
		void Set(const char *nom, bool val, const char *udesc, const char *ldesc) { 
			nombre=nom; default_value=val;
#ifdef FOR_WXPSEINT
			user_desc=_Z(udesc); long_desc=_Z(ldesc);
#endif
		}
	};
	static aux_struct data[LS_COUNT];
	static bool init_done;
	static void init();
	
	int version;
	std::string name;
	std::string descripcion;
	LS_SOURCE_ENUM source;
	bool settings[LS_COUNT];
	
	bool &operator[](LS_ENUM x) { return settings[x]; }
	bool operator[](LS_ENUM x) const { return settings[x]; }
	bool operator!=(const LangSettings &o) const {
		for(int i=0;i<LS_COUNT;i++)
			if (settings[i]!=o.settings[i]) return true;
		return false;
	}
	void Reset(int init_version = LS_VERSION) {
		if (!init_done) init();
		name = "<predeterminado>";
		source = LS_DEFAULT;
		descripcion.clear();
		version=init_version;
		for(int i=0;i<LS_COUNT;i++) 
			settings[i] = data[i].default_value;
		initKeywords(keywords);
	}
	LangSettings(LS_INIT_ENUM init_mode) { 
		if (init_mode==LS_DO_NOT_INIT) return; 
		Reset();
	}
	std::string GetAsSingleString() const;
	bool SetFromSingleString(const std::string &str);
	
#ifdef FOR_WXPSEINT
	bool Load(const wxString &fname, bool from_list);
	bool Save(const wxString &fname) const;
	void Log() const;
private:
#endif
	bool Load(const std::string &fname);
	bool Save(const std::string &fname)  const;
#ifdef FOR_WXPSEINT
public:
#endif

	bool IsTrue(const std::string &value) const {
		return (value.size() && (value[0]=='1' || value[0]=='V' || value[0]=='v' || value[0]=='s' || value[0]=='S' || value[0]=='T' || value[0]=='t'));
	}
	
	std::string GetConfigLine(int i) const {
		std::string ret(data[i].nombre); ret+="="; ret+=settings[i]?"1":"0"; return ret;
	}
	bool ProcessConfigLine(const std::string &key, const std::string &value) {
		if (key=="name") { name = value; return true; }
		else if (key=="desc") { descripcion+=value+"\n"; return true; }
		else if (key=="binprofile") { SetFromSingleString(value); return true; }
		else if (key=="version") { version=atoi(value.c_str()); return true; }
		else if (key=="source") { 
			if (value=="default")   source = LS_DEFAULT;
			else if (value=="list") source = LS_LIST;
			else if (value=="file") source = LS_FILE;
			else source = LS_CUSTOM;
			return true; 
		}
		else if (key=="use_nassi_schneiderman") return ProcessConfigLine("use_nassi_shneiderman",value); // estaba mal deletreado... sChneiderman
		else {
			for(int i=0;i<LS_COUNT;i++) { 
				if (key==data[i].nombre) {
					settings[i]=IsTrue(value);
					return true;
				}
			}
		}
		return false;
	}
	bool ProcessConfigLine(std::string line) {
		if (line.size()==0 || line[0]=='#') return false;
		size_t pos=line.find('='); if (pos==std::string::npos) return false;
		return ProcessConfigLine(line.substr(0,pos),line.substr(pos+1));
	}
	void Fix();
	
	std::string GetKeywords() const;
	std::string GetFunctions() const;
	
	KeywordsList keywords;
};

#endif
