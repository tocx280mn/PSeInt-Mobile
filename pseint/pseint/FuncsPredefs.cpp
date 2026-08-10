#include <cmath>
#include <ctime>
#include "FuncsManager.hpp"
#include "ErrorHandler.hpp"
#include "LangSettings.h"
#include "global.h"

DataValue func_rc(ErrorHandler &err_handler, DataValue *arg) {
	double x = arg[0].GetAsReal();
	if (x<0) {
		err_handler.ErrorIfRunning(147,"Raíz cuadrada de número negativo.");
		return DataValue::MakeEmpty(vt_numerica);
	} 
	return DataValue::MakeReal(std::sqrt(x));
}

DataValue func_abs(ErrorHandler &err_handler, DataValue *arg) {
	double d = arg[0].GetAsReal();
	return DataValue::MakeReal( d<0 ? -d : d );
}

DataValue func_ln(ErrorHandler &err_handler, DataValue *arg) {
	double x = arg[0].GetAsReal();
	if (x<=0) {
		err_handler.ErrorIfRunning(148,"Logaritmo de 0 o negativo.");
		return DataValue::MakeEmpty(vt_numerica);
	}
	return DataValue::MakeReal(log(x));
}
DataValue func_exp(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeReal(exp(arg[0].GetAsReal()));
}

DataValue func_sen(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeReal(sin(arg[0].GetAsReal()));
}

DataValue func_asen(ErrorHandler &err_handler, DataValue *arg) {
	double x = arg[0].GetAsReal();
	if (x<-1||x>+1) {
		err_handler.ErrorIfRunning(312,"Argumento inválido para la función ASEN (debe estar en [-1;+1]).");
		return DataValue::MakeEmpty(vt_numerica);
	}
	return DataValue::MakeReal(asin(x));
}

DataValue func_acos(ErrorHandler &err_handler, DataValue *arg) {
	double x = arg[0].GetAsReal();
	if (x<-1||x>+1) {
		err_handler.ErrorIfRunning(312,"Argumento inválido para la función ACOS (debe estar en [-1;+1]).");
		return DataValue::MakeEmpty(vt_numerica);
	}
	return DataValue::MakeReal(acos(x));
}

DataValue func_cos(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeReal(cos(arg[0].GetAsReal()));
}

DataValue func_tan(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeReal(tan(arg[0].GetAsReal()));
}

DataValue func_atan(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeReal(atan(arg[0].GetAsReal()));
}

DataValue func_azar(ErrorHandler &err_handler, DataValue *arg) {
	int x = arg[0].GetAsInt();
	if (x<=0) {
		err_handler.ErrorIfRunning(306,"Azar de 0 o negativo.");
		return DataValue::MakeEmpty(vt_numerica_entera);
	} else
		return DataValue::MakeInt(rand()%x);
}

DataValue func_fecha(ErrorHandler &err_handler, DataValue *arg) {
	std::time_t now = std::time(nullptr); std::tm *gt = std::gmtime(&now);
	int aa = gt->tm_year+1900, mm = gt->tm_mon+1, dd = gt->tm_mday;
	return DataValue::MakeInt(aa*10000+mm*100+dd);
}

DataValue func_hora(ErrorHandler &err_handler, DataValue *arg) {
	std::time_t now = std::time(nullptr); std::tm *gt = std::localtime(&now);
	int hh = gt->tm_hour, mm = gt->tm_min, ss = gt->tm_sec;
	return DataValue::MakeInt(hh*10000+mm*100+ss);
}

DataValue func_aleatorio(ErrorHandler &err_handler, DataValue *arg) {
	int a = arg[0].GetAsInt();
	int b = arg[1].GetAsInt();
	if (b<a) { int x=a; a=b; b=x; }
	return DataValue::MakeInt(a+rand() % (b-a+1));
}

DataValue func_trunc(ErrorHandler &err_handler, DataValue *arg) {
	double dbl = arg[0].GetAsReal();
	int i = int(dbl);
	// intentar compensar algunos errores numéricos... que al menos parezca el 
	// número que se ve al escribir, el cual se redondea a 10 decimales... 
	// por eso uso un epsilon un poquito más chico que eso
	if ((i+1)-dbl<1e-11) i++;      //  ((+56)+1)-(+56.999999) = +0.00001,   
	else if ((i-1)-dbl>-1e-11) i--; // ((-56)-1)-(-56.999999) = -0.00001
	return DataValue::MakeInt(i);
}

DataValue func_redon(ErrorHandler &err_handler, DataValue *arg) {
	double x = arg[0].GetAsReal();
	return DataValue::MakeInt(int(x+(x<0?-.5:+.5)));
}

DataValue func_longitud(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeInt(arg[0].GetAsString().size());
}

DataValue func_mayusculas(ErrorHandler &err_handler, DataValue *arg) {
	std::string s = arg[0].GetAsString(); size_t l = s.size();
	for(size_t i=0;i<l;i++) { 
		if (s[i]>='a'&&s[i]<='z') s[i]+='A'-'a';
		else if (s[i]=='ñ') s[i]='Ñ';
		else if (s[i]=='á') s[i]='Á';
		else if (s[i]=='é') s[i]='É';
		else if (s[i]=='í') s[i]='Í';
		else if (s[i]=='ó') s[i]='Ó';
		else if (s[i]=='ú') s[i]='Ú';
		else if (s[i]=='ü') s[i]='Ü';
	}
	return DataValue::MakeString(s);
}

DataValue func_minusculas(ErrorHandler &err_handler, DataValue *arg) {
	std::string s = arg[0].GetAsString(); size_t l = s.length();
	for(size_t i=0;i<l;i++) { 
		if (s[i]>='A'&&s[i]<='Z') s[i]+='a'-'A';
		else if (s[i]=='Ñ') s[i]='ñ';
		else if (s[i]=='Á') s[i]='á';
		else if (s[i]=='É') s[i]='é';
		else if (s[i]=='Í') s[i]='í';
		else if (s[i]=='Ó') s[i]='ó';
		else if (s[i]=='Ú') s[i]='ú';
		else if (s[i]=='Ü') s[i]='ü';
	}
	return DataValue::MakeString(s);
}

DataValue func_subcadena(ErrorHandler &err_handler, DataValue *arg) {
	std::string s = arg[0].GetAsString(); int l=s.length(), f=arg[1].GetAsInt(), t=arg[2].GetAsInt();
	if (!lang[LS_BASE_ZERO_ARRAYS]) { f--; t--; }
	if (t>l-1) t=l-1; 
	if (f<0) f=0;
	if (t<f) return DataValue::MakeEmpty(vt_caracter);
	return DataValue::MakeString(s.substr(f,t-f+1));
}

DataValue func_concatenar(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeString(arg[0].GetAsString()+arg[1].GetAsString());
}

DataValue func_atof(ErrorHandler &err_handler, DataValue *arg) {
	// verificar formato
	std::string s = arg[0].GetAsString();
	bool punto=false; int j=0;
	if (s.size() && (s[0]=='+'||s[0]=='-')) j++;
	for(unsigned int i=j;i<s.size();i++) {
		if (!punto && s[i]=='.')
			punto=true;
		else if (s[i]<'0'||s[i]>'9') {
			err_handler.ErrorIfRunning(311,std::string("La cadena (\"")+s+"\") no representa un número.");
			return DataValue::MakeEmpty(vt_numerica);
		}
	}
	// convertir
	return DataValue::MakeReal(s);
}

DataValue func_ftoa(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeString(arg[0].GetForUser()); // la conversión es para que redondee
}

DataValue func_pi(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeReal(3.141592653589793238462643383279502884197169399375105820974944592);
}
DataValue func_euler(ErrorHandler &err_handler, DataValue *arg) {
	return DataValue::MakeReal(2.7182818284590452353602874713527);
}

void FuncsManager::LoadPredefs() {
	m_predefs["PI"]        = std::make_unique<Funcion>(vt_numerica,func_pi); 
	m_predefs["EULER"]     = std::make_unique<Funcion>(vt_numerica,func_euler); 
	m_predefs["RC"]        = std::make_unique<Funcion>(vt_numerica,func_rc,vt_numerica); 
	m_predefs["RAIZ"]      = std::make_unique<Funcion>(vt_numerica,func_rc,vt_numerica); 
	m_predefs["ABS"]       = std::make_unique<Funcion>(vt_numerica,func_abs,vt_numerica);
	m_predefs["LN"]        = std::make_unique<Funcion>(vt_numerica,func_ln,vt_numerica);
	m_predefs["EXP"]       = std::make_unique<Funcion>(vt_numerica,func_exp,vt_numerica);
	m_predefs["SEN"]       = std::make_unique<Funcion>(vt_numerica,func_sen,vt_numerica);
	m_predefs["ASEN"]      = std::make_unique<Funcion>(vt_numerica,func_asen,vt_numerica);
	m_predefs["ACOS"]      = std::make_unique<Funcion>(vt_numerica,func_acos,vt_numerica);
	m_predefs["COS"]       = std::make_unique<Funcion>(vt_numerica,func_cos,vt_numerica);
	m_predefs["TAN"]       = std::make_unique<Funcion>(vt_numerica,func_tan,vt_numerica);
	m_predefs["ATAN"]      = std::make_unique<Funcion>(vt_numerica,func_atan,vt_numerica);
	m_predefs["AZAR"]      = std::make_unique<Funcion>(vt_numerica,func_azar,vt_numerica_entera);
	m_predefs["ALEATORIO"] = std::make_unique<Funcion>(vt_numerica,func_aleatorio,vt_numerica_entera,vt_numerica_entera);
	m_predefs["TRUNC"]     = std::make_unique<Funcion>(vt_numerica,func_trunc,vt_numerica);
	m_predefs["REDON"]     = std::make_unique<Funcion>(vt_numerica,func_redon,vt_numerica);
	if (lang[LS_ENABLE_STRING_FUNCTIONS]) {
		m_predefs["CONVERTIRANÚMERO"] = std::make_unique<Funcion>(vt_numerica,func_atof,vt_caracter);
		m_predefs["CONVERTIRANUMERO"] = std::make_unique<Funcion>(vt_numerica,func_atof,vt_caracter);
		m_predefs["CONVERTIRATEXTO"]  = std::make_unique<Funcion>(vt_caracter,func_ftoa,vt_numerica);
		m_predefs["LONGITUD"]         = std::make_unique<Funcion>(vt_numerica,func_longitud,vt_caracter);
		m_predefs["SUBCADENA"]        = std::make_unique<Funcion>(vt_caracter,func_subcadena,vt_caracter,vt_numerica_entera,vt_numerica_entera);
		m_predefs["MAYUSCULAS"]       = std::make_unique<Funcion>(vt_caracter,func_mayusculas,vt_caracter);
		m_predefs["MINUSCULAS"]       = std::make_unique<Funcion>(vt_caracter,func_minusculas,vt_caracter);
		m_predefs["MAYÚSCULAS"]       = std::make_unique<Funcion>(vt_caracter,func_mayusculas,vt_caracter);
		m_predefs["MINÚSCULAS"]       = std::make_unique<Funcion>(vt_caracter,func_minusculas,vt_caracter);
		m_predefs["CONCATENAR"]       = std::make_unique<Funcion>(vt_caracter,func_concatenar,vt_caracter,vt_caracter);
	}
	m_predefs["FECHAACTUAL"]     = std::make_unique<Funcion>(vt_numerica,func_fecha);
	m_predefs["HORAACTUAL"]      = std::make_unique<Funcion>(vt_numerica,func_hora);
}
