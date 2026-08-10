#include <iostream>
#include <wx/txtstrm.h>
#include "RTSyntaxManager.h"
#include "mxProcess.h"
#include "ConfigManager.h"
#include "mxMainWindow.h"
#include "mxVarWindow.h"
#include "ids.h"
#include "Logger.h"
#include "string_conversions.h"

RTSyntaxManager *RTSyntaxManager::the_one = nullptr;

RTSyntaxManager::RTSyntaxManager():wxProcess(wxPROCESS_REDIRECT),conv("ISO-8851") {
	_LOG("RTSyntaxManager::RTSyntaxManager");
	processing=running=restart=false;
	timer = new wxTimer(main_window->GetEventHandler(),mxID_RT_TIMER);
}

void RTSyntaxManager::Start ( ) {
	_LOG("RTSyntaxManager::Start");
	if (the_one) {
		if (the_one->running) return;
		delete the_one;
	}
	the_one=new RTSyntaxManager;
	the_one->restart=false;
	wxString command;
	command<<config->pseint_command<<" --forrealtimesyntax "<<mxProcess::GetProfileArgs();
	the_one->pid=wxExecute(command,wxEXEC_ASYNC,the_one);
	_LOG("RTSyntaxManager::Start  pid="<<the_one->pid<<"  command="<<command);
}

void RTSyntaxManager::Stop ( ) {
	_LOG("RTSyntaxManager::Stop");
	if (the_one && the_one->pid<=0) return;
	if (the_one) { the_one->Kill(the_one->pid,wxSIGKILL); the_one->src=NULL; }
}

void RTSyntaxManager::Restart ( ) {
	if (!the_one) Start();
	else { the_one->restart=true; Stop(); }
}

bool RTSyntaxManager::Process (mxSource *src, std::function<void()> &&action_post) {
	if (!src) { 
		if (the_one && the_one->processing) { the_one->ContinueProcessing(); return true; }
		_LOG("RTSyntaxManager::Process ERROR: the_one->Process(NULL) && (!the_one || !the_one->processing): the_one="<<the_one);
		return false; // no deberia pasar (solo si no puede lanzar el interprete o revienta enseguida)
	}
	if (!the_one) Start(); else if (the_one->processing || the_one->restart) return false;
	_LOG("RTSyntaxManager::Process in src="<<src);
	the_one->src = src; the_one->m_action_post = action_post;
	the_one->m_current_step = Step::SendCode;
	wxTextOutputStream output(*(the_one->GetOutputStream())
#ifdef UNICODE
							  ,wxEOL_NATIVE,the_one->conv
#endif
							  );
	the_one->processing = true;
	for(int i=0; i<src->GetLineCount(); ++i) {
		wxString s=src->GetLine(i); src->ToRegularOpers(s); src->FixExtraUnicode(s);
		while ( (not s.IsEmpty()) and (s.Last()=='\r' or s.Last()=='\n') ) s.RemoveLast();
		output<<s<<"\n"; wxMilliSleep(0); // el millisleep evita problemas cuando se manda mucho y no alcanza a "flushear" (en wx 2.9 ya hay un .flush(), tendria que probar con eso, en 2.8 no hay caso)
	}
	output<<"<!{[END_OF_INPUT]}!>\n";
	src->RTOuputStarts();
	the_one->m_current_step = Step::ReadErrors;
	vars_window->BeginInput();
	the_one->ContinueProcessing();
	_LOG("RTSyntaxManager::Process out src="<<src);
	return true;
}

void RTSyntaxManager::ContinueProcessing() {
	_LOG("RTSyntaxManager::ContinueProcessing in src="<<src);
	wxTextInputStream input(*(GetInputStream()));	
	if (!src) { // si se cerro el fuente que estabamos analizando
		_LOG("RTSyntaxManager::ContinueProcessing out src="<<src);
		while (IsInputAvailable()) { /*char c=*/input.GetChar(); }
		return;
	}
	while(true) {
		static std::string aux_line; aux_line.clear();
		while (IsInputAvailable()) {
			char c=input.GetChar();
			if (c=='\n') break;
			if (c!='\r') aux_line+=c;
		}
		wxString line(_Z(aux_line.c_str()));
		if (not line.IsEmpty()) {
			switch(m_current_step) {
				case Step::None: _impossible; break;
				case Step::SendCode: _impossible; break;
				case Step::ReadErrors:
					if (line=="<!{[END_OF_OUTPUT]}!>") { 
						_LOG("RTSyntaxManager::ContinueProcessing fase 1 src="<<src);
						m_current_step = Step::ReadVars;
					} else 
						if (config->rt_syntax) src->MarkError(line);
					break;
				case Step::ReadVars:
					if (line=="<!{[END_OF_VARS]}!>") {
						_LOG("RTSyntaxManager::ContinueProcessing fase 2 src="<<src);
						vars_window->EndInput();
						m_current_step = Step::ReadBlocks;
					} else {
						wxString what=line.BeforeFirst(' ');
						if (config->show_vars || m_action_post) {
							if (what=="PROCESO"||what=="SUBPROCESO")
								vars_window->AddProc(line.AfterFirst(' '),what=="PROCESO");
							else
								vars_window->AddVar(what,line.Last());
						} else {
							if (what=="SUBPROCESO") 
								vars_window->RegisterAutocompKey(line.AfterFirst(' ').BeforeFirst(':'));
							else if (what!="PROCESO")
								vars_window->RegisterAutocompKey(what.BeforeFirst('['));
						}
						if (what=="PROCESO")
							src->SetMainProcessTitleFromRTSM(line.AfterFirst(' ').BeforeFirst(':'));
					}
					break;
				case Step::ReadBlocks:
					if (line=="<!{[END_OF_BLOCKS]}!>") {
						processing=false;
						_LOG("RTSyntaxManager::ContinueProcessing out end src="<<src);
						src->RTOuputEnds();
						m_current_step = Step::PostAction;
						if (m_action_post) { m_action_post(); m_action_post = {}; }
						m_current_step = Step::None;
						if (config->highlight_blocks) src->HighLightBlock();
						return;
					} else if (config->highlight_blocks) {
						long l1,l2;
						if (line.BeforeFirst(' ').ToLong(&l1) && line.AfterFirst(' ').ToLong(&l2)) 
							src->AddBlock(l1-1,l2-1);
					}
					break;
				case Step::PostAction: _impossible; break;
			}
		} else {
			_LOG("RTSyntaxManager::ContinueProcessing out continue src="<<src);
			timer->Start(100,true);
			return;
		}
	}
}

void RTSyntaxManager::OnTerminate (int pid, int status) {
	_LOG("RTSyntaxManager::OnTerminate src="<<src);
	if (restart) {
		Start(); 
		main_window->UpdateRealTimeSyntax();
	} else 
		the_one=NULL;
}

bool RTSyntaxManager::IsLoaded ( ) {
	return the_one!=NULL;
}

RTSyntaxManager::~RTSyntaxManager ( ) {
	timer->Stop();
	the_one=NULL;
}

void RTSyntaxManager::OnSourceClose(mxSource *_src) {
	if (!the_one) return;
	if (the_one->src==_src) {
		the_one->src=NULL;
		the_one->ContinueProcessing();
	}
}
