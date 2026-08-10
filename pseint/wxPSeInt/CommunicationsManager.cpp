#include <wx/socket.h>
#include <iostream>
#include "CommunicationsManager.h"
#include "mxMainWindow.h"
#include "ConfigManager.h"
#include "mxHelpWindow.h"
#include "DebugManager.h"
#include "mxStatusBar.h"
#include "mxDebugWindow.h"
#include "Logger.h"

enum {MXS_TYPE_UNKNOWN, MXS_TYPE_DEBUG, MXS_TYPE_FLOW, MXS_TYPE_RUN};

CommunicationsManager *comm_manager = NULL;


mxSocketClient::mxSocketClient(wxSocketBase *s)
	: type(MXS_TYPE_UNKNOWN),src_id(-1),socket(s),
	  is_working(0), should_delete(false)
{
	_LOG("mxSocketClient::mxSocketClient "<<this);
	socket->SetEventHandler(*(main_window->GetEventHandler()), wxID_ANY);
	socket->SetNotify(wxSOCKET_LOST_FLAG|wxSOCKET_INPUT_FLAG);
	socket->Notify(true);
}

mxSocketClient::~mxSocketClient() {
	_LOG("mxSocketClient::~mxSocketClient "<<this);
	if (socket) delete socket;
}

void mxSocketClient::ProcessLost() {
	if (type==MXS_TYPE_DEBUG) debug->ProcessSocketLost();
	else if (type==MXS_TYPE_FLOW) ProcessLostFlow();
	else if (type==MXS_TYPE_RUN) ProcessLostRun();
	else _LOG("mxSocketClient::ProcessLost error");
}

void mxSocketClient::ProcessLostRun() {
	mxSource *src=main_window->FindSourceById(src_id);
	if (src) src->SetRunSocket(NULL);
}

void mxSocketClient::ProcessLostFlow() {
	mxSource *src=main_window->FindSourceById(src_id);
	if (src) src->SetFlowSocket(NULL);
}

void mxSocketClient::ProcessInput (wxSocketEvent & evt) {
	is_working++;
	// leer datos y procesar
	char buf[256];
	socket->Read(buf,255);
	int n = socket->LastCount();
	while (n>0) {
		buf[n]='\0';
		char *aux=buf;
		for (int i=0;i<n;i++)
			if (buf[i]=='\n') {
				buf[i]='\0';
				buffer<<_Z(aux);
				ProcessCommand();
				buffer=_T("");
				aux=buf+i+1;
			}
		buffer<<aux;
		socket->Read(buf,255);
		n = socket->LastCount();
	}
	is_working--;
}

void mxSocketClient::ProcessCommand ( ) {
	if (type==MXS_TYPE_DEBUG) debug->ProcSocketData(buffer);
	else if (type==MXS_TYPE_FLOW) ProcessCommandFlow();
	else if (type==MXS_TYPE_RUN) ProcessCommandRun();
	else if (buffer.StartsWith("hello-")) {
		wxString stype=buffer.AfterFirst('-').BeforeFirst(' ');
		if (stype=="flow") {
			long id=-1; if (!buffer.AfterFirst(' ').ToLong(&id)) { _LOG("mxSocketClient::ProcessCommand hello-id error"); return; } src_id=id;
			type=MXS_TYPE_FLOW;
			mxSource *src=main_window->FindSourceById(src_id);
			if (src) src->SetFlowSocket(socket);
		} else if (stype=="run") {
			long id=-1; if (!buffer.AfterFirst(' ').ToLong(&id)) { _LOG("mxSocketClient::ProcessCommand hello-id error"); return; } src_id=id;
			type=MXS_TYPE_RUN;
			mxSource *src=main_window->FindSourceById(src_id);
			if (src) src->SetRunSocket(socket);
		} else if (stype=="debug") {
			type=MXS_TYPE_DEBUG;
			debug->SetSocket(socket);
		}
		else { _LOG("mxSocketClient::ProcessCommand hello-type error"); return; }
		
	}
}

void mxSocketClient::ProcessCommandRun() {
	if (buffer.StartsWith("location ")) {
		mxSource *src=main_window->FindSourceById(src_id);
		buffer=buffer.AfterFirst(' ');
		long line,inst; 
		if (src && buffer.BeforeFirst(':').ToLong(&line) && buffer.AfterFirst(':').ToLong(&inst))
			src->SelectInstruccion(line-1,inst-1);
	} else if (buffer=="terminated") {
		mxSource *src=main_window->FindSourceById(src_id);
		if (src) main_window->ParseResults(src);
	} else if (buffer=="activated") {
		mxSource *src=main_window->FindSourceById(src_id);
		if (src && src->GetStatus()==STATUS_RUNNING_CHANGED) src->UpdateRunningTerminal(false);
	} else if (buffer=="step") {
		wxCommandEvent evt;
		if (debug->debugging) debug_panel->OnDebugStep(evt);
	}
}

void mxSocketClient::ProcessCommandFlow() {
	if (buffer=="help") {
		if (!helpw) helpw = new mxHelpWindow();
		helpw->ShowHelp(_T("flow-quick.html"));
		return;
	}
	mxSource *src=main_window->FindSourceById(src_id);
	if (!src) { _LOG("mxSocketClient::ProcessCommandFlow src error"); return; }
	if (buffer=="modified") {
		if (src==main_window->GetCurrentSource()) 
			src->SetStatus(STATUS_FLOW_CHANGED);
	} else if (buffer=="update"||buffer=="reload"||buffer=="run"||buffer=="export"||buffer=="debug") {
		src->ReloadFromTempPSD(buffer=="update"||buffer=="reload");
		if (buffer=="update") return;
		main_window->SelectSource(src);
		if (buffer=="run") main_window->RunCurrent(true,true);
		else if (buffer=="debug") { wxCommandEvent evt; debug_panel->DebugStartFromGui(true); }
		else if (buffer=="export") { wxCommandEvent evt; main_window->OnRunSaveFlow(evt); }
	} else if (buffer.StartsWith("config:")) {
		wxString key = buffer.AfterFirst(':').BeforeFirst('=');
		bool val = buffer.Last()=='1';
		if      (key=="ns")    config->GetWritableLang()[LS_USE_NASSI_SHNEIDERMAN] = val;
		else if (key=="dark")  config->use_dark_psdraw = val;
		else if (key=="color") config->shape_colors = val;
		else if (key=="crop")  config->psdraw_nocrop = !val;
		main_window->UpdatePSDrawSettings();
	}
}

CommunicationsManager::CommunicationsManager() {
	server_port = -1;
	server = NULL;
	_LOG("CommunicationsManager::CommunicationsManager in "<<server_port);
	do {
		wxIPV4address adrs;
#ifndef __APPLE__
		// esto puede traer problemas (al menos se cuelga en mi mac de vbox), 
		// y localhost:0 es el valor por default supestamente, asi que no 
		// deberia molestar no ponerlo, pero si no lo pongo en Windows salta
		// el firewall
		adrs.Hostname("localhost"); 
#endif
		adrs.Service(server_port=config->GetCommPort());
		if (server) delete server;
		server = new wxSocketServer(adrs,wxSOCKET_NOWAIT);
		server->SetEventHandler(*(main_window->GetEventHandler()), wxID_ANY);
		server->SetNotify(wxSOCKET_CONNECTION_FLAG);
		server->Notify(true);
	} while (!server->IsOk());
	_LOG("CommunicationsManager::CommunicationsManager out port="<<server_port<<"  server="<<server);
}

void CommunicationsManager::SocketEvent(wxSocketEvent &event) {
	wxSocketBase *s=event.GetSocket();
	if (s==server) { // si se pedia una nueva conexion, se crea un socket en sockets
		clients.push_back(new mxSocketClient(server->Accept(false)));
		_LOG("CommunicationsManager::SocketEvent::NewClient s="<<clients.back());
	} else if (event.GetSocketEvent()==wxSOCKET_INPUT) { // si son datos de entrada...
		auto it1=clients.begin(), it2=clients.end();
		while (it1!=it2) {
			if ((**it1)==s) { 
				(*it1)->ProcessInput(event);
				if ((*it1)->should_delete && (*it1)->is_working==0) 
					{ delete *it1; clients.erase(it1); }
				return;
			}
			++it1;
		}
		_LOG("CommunicationsManager::SocketEvent::Input error");
	} else if (event.GetSocketEvent()==wxSOCKET_LOST) { // si por algo anormal se corto una conexion pendiente, liberar en sockets
		_LOG("CommunicationsManager::SocketEvent::Lost s="<<s);
		auto it1=clients.begin(), it2=clients.end();
		while (it1!=it2) {
			if ((**it1)==s) {
				(*it1)->ProcessLost();
				// para evitar que un evento lost se procese mientras se esta procesando un input y entonces el lost lo destruya y el input no pueda continuar
				if ((*it1)->is_working!=0) (*it1)->should_delete=true;
				else { delete *it1; clients.erase(it1); }
				return;
			}
			++it1;
		}
		_LOG("CommunicationsManager::SocketEvent::Lost error");
	}
}

int CommunicationsManager::GetServerPort ( ) {
	return server_port;
}

CommunicationsManager::~CommunicationsManager() {
	comm_manager = NULL; server->Destroy();
}

