#ifndef COMMUNICATIONSMANAGER_H
#define COMMUNICATIONSMANAGER_H

#include <wx/string.h>
#include <list>
class wxSocketBase;
class wxSocketServer;
class wxSocketEvent;
class wxObject;

class mxSocketClient {
	int type, src_id; 
	wxSocketBase *socket;
	wxString buffer;
	friend class CommunicationsManager;
	int is_working;
	bool should_delete; 
public:
	mxSocketClient(wxSocketBase *s);
	~mxSocketClient();
	void ProcessInput(wxSocketEvent &evt);
	void ProcessLost();
	void ProcessLostFlow();
	void ProcessLostRun();
	bool operator==(const wxSocketBase* s) { return s==socket; }
	void ProcessCommand();
	void ProcessCommandRun();
	void ProcessCommandFlow();
};

class CommunicationsManager {
	
private:
	// servidor
	int server_port;
	wxSocketServer *server; // el servidor, todos los pedidos de comunicacion se hacen aqui
	std::list<mxSocketClient*> clients; // clientes (se crean aca y en cuanto se sepa para que se crearon se donan al mxSource o al DebugManager)
public:
	CommunicationsManager();
	~CommunicationsManager();
	void SocketEvent(wxSocketEvent &event);
	int GetServerPort();
};

extern CommunicationsManager *comm_manager;

#endif

