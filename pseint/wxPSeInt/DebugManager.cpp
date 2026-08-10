#include <iostream>
#include "DebugManager.h"
#include "mxSource.h"
#include "mxMainWindow.h"
#include "mxProcess.h"
#include "mxDesktopTestGrid.h"
#include "mxEvaluateDialog.h"
#include "ConfigManager.h"
#include "mxDebugWindow.h"
#include "mxSubtitles.h"
#include "string_conversions.h"
#include "Logger.h"
#include "mxBacktrace.h"

DebugManager *debug = NULL;

DebugManager::DebugManager() {
	do_desktop_test=false;
	debugging=false;
	step_in=true;
	subtitles_on=false;
	source=NULL;
}

void DebugManager::Start(mxProcess *proc, mxSource *src) {
	socket=NULL;
	current_proc_name=_Z("<desconocido>");
	process = proc;
	sbuffer="";
	source=src;
	debugging=true;
	paused=false;
	on_evaluar.clear();
	subtitles->Reset();
	desktop_test->ResetTest();
	backtrace->Reset();
	return;
	
}

void DebugManager::ProcSocketData(wxString data) {
	if (data.StartsWith("subtitulo ")) {
		subtitles->AddMessage(current_line,current_inst,data.Mid(10));
	} if (data=="finalizando") {
		backtrace->AboutToPop();
	} if (data.StartsWith("proceso ")) {
		current_proc_name=data.Mid(8);
		backtrace->Push(current_proc_name.AfterFirst(':'),current_line);
	} else if (data.StartsWith("linea ")) {
		long l=-1,i=-1;
		if (data.Contains(":")) {
			data.Mid(6).BeforeFirst(':').ToLong(&l);
			data.AfterLast(':').ToLong(&i);
		} else {
			data.Mid(6).ToLong(&l);
		}
		if (l>=0 && source!=NULL) source->SetDebugLine(l-1,i-1);
		if (do_desktop_test) desktop_test->SetLine(current_proc_name,l,i);
		current_line=l; current_inst=i;
	} else if (data.StartsWith("autoevaluacion ")) {
		long l=-1;
		data.Mid(15).BeforeFirst(' ').ToLong(&l);
		desktop_test->SetAutoevaluation(l,data.Mid(15).AfterFirst(' '));
	} else if (data.StartsWith("estado ")) {
		wxString state = data.AfterFirst(' ');
		if (state=="inicializado") {
			// cargar la prueba de escritorio
			do_desktop_test=debug_panel->IsDesktopTestEnabled();
			if (do_desktop_test) {
				const wxArrayString &vars  = desktop_test->GetDesktopVars();
				for (unsigned int i=0;i<vars.GetCount();i++) {
					wxString str(wxString("autoevaluar ")<<vars[i]<<"\n");
					socket->Write(str.c_str(),str.Len());
				}
			}
			// configurar el tipo de paso a paso (step in o step over)
			wxString str1("subprocesos "); str1<<(step_in?1:0)<<"\n";
			socket->Write(str1.c_str(),str1.Len());
			wxString str2("subtitulos "); str2<<(subtitles_on?1:0)<<"\n";
			socket->Write(str2.c_str(),str2.Len());
			// iniciar la ejecución
			if ((paused=should_pause)) {
				wxString str("paso\n");
				socket->Write(str.c_str(),str.Len());
			}	else {
				wxString str("comenzar\n");
				socket->Write(str.c_str(),str.Len());
			}
		} else if (state=="pausa") {
			debug_panel->SetState(DS_PAUSED);
			if (source) source->SetDebugPause();
		} else if (state=="paso")
			debug_panel->SetState(DS_STEP);
		else if (state=="ejecutando")
			debug_panel->SetState(DS_RESUMED);
		else if (state=="finalizado") {
			debug_panel->SetState(DS_FINALIZED);
			if (source) source->SetDebugLine();
		} else {
			if (source) source->SetDebugLine();
			debug_panel->SetState(DS_NONE);
		}
	} else if (data.StartsWith("evaluacion ")) {
		RunLambda(data.AfterFirst(' ').BeforeFirst(' '),data.AfterFirst(' ').AfterFirst(' '));
	}	
}

void DebugManager::Close(mxSource *src) {
	if (source==src) {
		if (debugging) Stop();
		source=NULL;
	}
}

void DebugManager::SetSpeed(int speed) {
	if (debugging && socket) {
		int sp = _calc_delay(speed);
		wxString str("delay ");
		str<<sp<<'\n';
		socket->Write(str.c_str(),str.Len());
	}
}

int DebugManager::GetSpeed(int sp) const {
	return 100-((sp-25)/20);
}

void DebugManager::Step() {
	if (debugging && socket) {
		wxString str("paso\n");
		socket->Write(str.c_str(),str.Len());
	}
}

bool DebugManager::Pause() {
	paused = !paused;
	if (debugging && socket) {
		wxString str;
		if (paused) str="pausar\n";
		else str="continuar\n";
		socket->Write(str.c_str(),str.Len());
	}
	return paused;
}

void DebugManager::Stop() {
	backtrace->Reset();
	if (process && process->pid!=0 && process->Exists(process->pid)) {
		wxProcess::Kill(process->pid,wxSIGKILL,wxKILL_CHILDREN);
	}
}

static wxString aux_SendEvaluation_GetNextKey() {
	static wxString key = "@";
	int a=0; 
	while( (key[a]=_C(key[a])+1)=='Z' ) {
		key[a++]='A';
		if (a==int(key.length())) key+="@";
	}
	return key;
}

void DebugManager::SendEvaluation(wxString exp, DebugLambda *lambda) {
	wxString key = aux_SendEvaluation_GetNextKey();
	on_evaluar[key] = lambda;
	wxString str("evaluar ");
	str<<key<<" "<<exp<<"\n";
	socket->Write(str.c_str(),str.Len());	
}

void DebugManager::SetStepIn(bool b) {
	step_in=b;
	if (debugging && socket) {
		wxString str("subprocesos "); str<<(step_in?1:0)<<"\n";
		socket->Write(str.c_str(),str.Len());
	}
}

void DebugManager::SetSubtitles(bool b) {
	subtitles_on=b;
	if (debugging && socket) {
		wxString str("subtitulos "); str<<(b?1:0)<<"\n";
		socket->Write(str.c_str(),str.Len());
	}
}

void DebugManager::SetSocket (wxSocketBase * s) {
	_LOG("DebugManager::SetSocket socket="<<s);
	socket=s;
}

void DebugManager::ProcessSocketLost() {
	_LOG("DebugManager::SocketLost socket=nullptr");
	socket=NULL;
}

void DebugManager::InvalidateLambda (void * ptr) {
	for (auto it=on_evaluar.begin(), end=on_evaluar.end(); it!=end; ) {
		if (it->second->Invalidate(ptr)) { auto aux = it++; on_evaluar.erase(aux); } else ++it;
	}
}

void DebugManager::RunLambda (wxString key, wxString ans) {
	auto it=on_evaluar.find(key);
	if (it==on_evaluar.end()) return;
	it->second->Do(ans); delete it->second; on_evaluar.erase(it);
}

