#include "intercambio.h"
#include <cstdlib>
#include "common.h"
#include "Evaluar.hpp"
#include "utils.h"
#include "SynCheck.hpp"
using namespace std;

Intercambio Inter;

string IntToStr(int f);

// *********************** Intercambio ****************************

// Linea que se esta ejecutando actualmente
Intercambio::Intercambio() {
	subtitles_on=false;
	backtraceLevel=0;
	debugLevel=0;
#ifdef USE_ZOCKETS
	is_evaluation_error=false;
	evaluating_for_debug=false;
	zocket = ZOCKET_ERROR;
	port=24377; 
#endif
	delay=0; do_continue=false; 
	do_one_step=false;
}
Intercambio::~Intercambio() {
#ifdef USE_ZOCKETS
	if (zocket!=ZOCKET_ERROR) 
		zocket_cerrar (zocket);
#endif
}
void Intercambio::UnInit() {
#ifdef USE_ZOCKETS
	if (zocket!=ZOCKET_ERROR) 
		zocket_cerrar(zocket);
	zocket=ZOCKET_ERROR;
#endif
}
#ifdef USE_ZOCKETS
void Intercambio::ProcData(string order) {
//	cout<<"DEBUG: "<<order<<endl;
	if (order=="paso" || order=="comenzar") {
		do_continue=true;
		if (order=="paso"||subtitles_on) {
			do_one_step=true;
			zocket_escribir(zocket,"estado paso\n",12);
		} else {
			do_one_step=false;
			zocket_escribir(zocket,"estado ejecutando\n",18);
		}
	} else if (order=="subtitulos 1") { // se mete en todos los subprocesos
		subtitles_on=true; 
		if (do_continue) { 
			do_continue=false;
			zocket_escribir(zocket,"estado pausa\n",13);
		}
	} else if (order=="subtitulos 0") { // no se mete más alla del subproceso actual
		subtitles_on=false;
	} else if (order=="subprocesos 1") { // se mete en todos los subprocesos
		debugLevel=0;
	} else if (order=="subprocesos 0") { // no se mete más alla del subproceso actual
		debugLevel=backtraceLevel;
		if (!debugLevel) debugLevel=1;
	} else if (order=="pausar") {
		do_continue=false;
		zocket_escribir(zocket,"estado pausa\n",13);
	} else if (order=="continuar") {
		do_continue=true; do_one_step=subtitles_on;
		zocket_escribir(zocket,"estado ejecutando\n",18);
	} else if (order.substr(0,6)=="delay ") {
		delay=atoi(order.substr(6).c_str());
	} else if (order.substr(0,8)=="evaluar ") { // "evaluar key value"
		is_evaluation_error=false; evaluating_for_debug=true; // evaluador en modo debug
		string str="evaluacion ";
		DataValue res;
		int key_end = 8; while(order[key_end]!=' ') ++key_end; // cortar clave
		str+= order.substr(8,(++key_end)-8);  // agregar clave en la respuesta 
		int pos_tipo = str.size(); str+="x "; // agregar espacio para el tipo
		string exp = order.substr(key_end); // cortar expresion
		ParseInspection(*m_runtime,exp); // reformatear
		if (!is_evaluation_error) // si parecia corrcta
			res = Evaluar(*m_runtime,exp); // evaluar
		if (is_evaluation_error) // si no se pude evaluar
			str+=evaluation_error+"\n"; // responder con error
		else { // si se pudo evaluar
			str += res.GetForUser()+"\n"; // agregar la expresion
		}
		str[pos_tipo]='0'+(res.type.cb_log?1:0)+(res.type.cb_num?2:0)+(res.type.cb_car?4:0); // reemplaza la x por el tipo
		zocket_escribir(zocket,str.c_str(),str.size()); // enviar respuesta
		evaluating_for_debug=false; // quitar evaluador del modo debug
	} else if (order.substr(0,12)=="autoevaluar ") {
		string exp = order.substr(12);
		evaluating_for_debug=true;
		is_evaluation_error=false;
		ParseInspection(*m_runtime,exp);
		if (is_evaluation_error) {
			autoevaluaciones_valid.push_back(false);
			autoevaluaciones.push_back(evaluation_error);
		} else {
			autoevaluaciones_valid.push_back(true);
			autoevaluaciones.push_back(exp);
		}
		evaluating_for_debug=false;
	} else if (order=="quit") {
		exit(0);
	}
}
#endif

void Intercambio::SendSubtitle(string _str) {
#ifdef USE_ZOCKETS
	if (zocket!=ZOCKET_ERROR && (!debugLevel || backtraceLevel<=debugLevel)) {
		string ss="subtitulo "; ss+=_str+"\n";
		zocket_escribir(zocket,ss.c_str(),ss.size());
	}
#endif
}

void Intercambio::SendPositionToGUI () {
#ifdef USE_ZOCKETS
	if (zocket!=ZOCKET_ERROR && (!debugLevel || backtraceLevel<=debugLevel)) { // si estamos depurando, informar la linea y esperar
		string str;
		if (loc.instruccion>0) str=string("linea ")+IntToStr(loc.linea)+":"+IntToStr(loc.instruccion)+"\n";
		else str=string("linea ")+IntToStr(loc.linea)+"\n";
		zocket_escribir(zocket,str.c_str(),str.size());
	}
#endif	
}

void Intercambio::SendIOPositionToTerminal (int argNumber) {
	cout<<"\033[zp"<<loc.linea<<':'<<loc.instruccion<</*':'<<argNumber<<*/	';';
}

void Intercambio::SendErrorPositionToTerminal () {
	cout<<"\033[ze"<<loc.linea<<':'<<loc.instruccion<<';';
}


void Intercambio::ChatWithGUI () {
#ifdef USE_ZOCKETS
	if (zocket!=ZOCKET_ERROR  && (!debugLevel || backtraceLevel<=debugLevel)) {
		evaluating_for_debug=true; string str;
		for (unsigned int i=0;i<autoevaluaciones.size();i++) {
			if (autoevaluaciones_valid[i]) {
				stringstream autoevaluacion;
				is_evaluation_error = false;
				DataValue res = Evaluar(*m_runtime,autoevaluaciones[i]);
				if (is_evaluation_error)
					autoevaluacion<<"autoevaluacion "<<i+1<<' '<<evaluation_error<<'\n';
				else {
					autoevaluacion<<"autoevaluacion "<<i+1<<' '<<res.GetForUser()<<'\n';
				}
				str = autoevaluacion.str();
				zocket_escribir(zocket,str.c_str(),str.size());
			} else {
				stringstream autoevaluacion;
				autoevaluacion<<"autoevaluacion "<<i+1<<' '<<autoevaluaciones[i]<<'\n';
				str = autoevaluacion.str();
				zocket_escribir(zocket,str.c_str(),str.size());
			}
		}
		evaluating_for_debug=false;
		if (do_one_step) { 
			zocket_escribir(zocket,"estado pausa\n",13); 
			do_continue=false;
			do { ProcInput(); } while (!do_continue);
		} else {
			ProcInput();
			if (do_continue) Sleep(delay);
		}
		do { ProcInput(); } while (!do_continue);
	}
#endif
}

#ifdef USE_ZOCKETS
void Intercambio::ProcInput() {
	static char buf[256];
	int n=255;
	while (zocket_leer(zocket,buf,n)) {
		if (n>0) {
			char *aux=buf;
			buf[n]='\0';
			for (int i=0;i<n;i++)
				if (buf[i]=='\n' || buf[i]=='\r') {
					buf[i]='\0';
					sbuffer+=aux;
					if (sbuffer.size())
						ProcData(sbuffer);
					sbuffer="";
					aux=buf+i+1;
				}
			if (aux<buf+n)
					sbuffer+=aux;
		}
		n=255;
	}
	Sleep(15); // para evitar que consuma 100% de cpu mientras no hace nada
}
#endif

void Intercambio::SetStarted() {
	running = true;
	backtraceLevel=0;
	backtrace.clear();
}

void Intercambio::SetFinished(bool interrupted) {
	backtraceLevel=0;
#ifdef USE_ZOCKETS
	if (zocket!=ZOCKET_ERROR) {
		if (!interrupted)
			zocket_escribir(zocket,"estado finalizado\n",18);
		else
			zocket_escribir(zocket,"estado interrumpido\n",20);
	}
#endif
	running = false;
}

#ifdef USE_ZOCKETS

void Intercambio::SetPort(int p) { 
	port = p; 
}

void Intercambio::InitDebug(RunTime &runtime, int _delay) {
	m_runtime = &runtime;
	if ( (delay=_delay) ) {
		zocket = zocket_llamar(port);
		if (zocket==ZOCKET_ERROR) 
			cerr<<"<<Error al inicializar ejecucion paso a paso>>"<<endl;
		else {
			zocket_escribir(zocket,"hello-debug -1\n",15);
			zocket_escribir(zocket,"estado inicializado\n",20);
			while (!do_continue) ProcInput();
		}
	}
}
#endif

// *****************************************************************

bool Intercambio::EvaluatingForDebug() {
#ifdef USE_ZOCKETS
	return evaluating_for_debug;
#else
	return false;
#endif
}

void Intercambio::SetError(string error) {
#ifdef USE_ZOCKETS
	is_evaluation_error = true;
	evaluation_error = error;
#endif
}

void Intercambio::OnFunctionIn(string nom) {
	backtrace.push_back(FrameInfo(nom,loc));
	backtraceLevel++;
#ifdef USE_ZOCKETS
	if (zocket==ZOCKET_ERROR || backtrace.empty()) return;
	string s="proceso "+IntToStr(backtraceLevel)+":"+backtrace.back().func_name+"\n";
	zocket_escribir(zocket,s.c_str(),s.size());
#endif	
}
void Intercambio::OnFunctionOut() {
	backtrace.pop_back();
	if (backtraceLevel==debugLevel) debugLevel--;
	backtraceLevel--;
#ifdef USE_ZOCKETS
	if (zocket==ZOCKET_ERROR || backtrace.empty()) return;
	string s="proceso "+IntToStr(backtraceLevel)+":"+backtrace.back().func_name+"\n";
	zocket_escribir(zocket,s.c_str(),s.size());
#endif
}

void Intercambio::OnAboutToEndFunction() {
#ifdef USE_ZOCKETS
	if (zocket==ZOCKET_ERROR || backtrace.empty()) return;
	string s="finalizando\n";
	zocket_escribir(zocket,s.c_str(),s.size());
#endif
}

int Intercambio::GetBacktraceLevel ( ) {
	return backtraceLevel;
}

FrameInfo Intercambio::GetFrame(int level) {
	return backtrace[level];
}

void Intercambio::SetLocation(CodeLocation _loc) {
	prev_loc = loc;
	loc.linea = _loc.linea; 
	loc.instruccion = _loc.instruccion; 
}

