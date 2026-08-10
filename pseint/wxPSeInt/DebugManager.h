#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H
#include <wx/string.h>
#include <map>

class mxSource;
class wxSocketBase;
class wxSocketServer;
class wxSocketEvent;
class mxProcess;
class mxEvaluateDialog;
class wxObject;


class DebugLambda { 
public: 
	virtual void Do(wxString ans)=0; /// action to perform on pause
	virtual bool Invalidate(void *ptr){ return false; } /// to avoid action on deleted objects
	virtual ~DebugLambda(){}
};

#define _DEBUG_LAMBDA_0(Name,Action) \
	class Name : public DebugLambda {\
	public: void Do(wxString ans) Action };

#define _DEBUG_LAMBDA_1(Name,PtrType,Arg,Action) \
	class Name : public DebugLambda {\
	public: Name(PtrType *arg) : Arg(arg) {} \
	public: void Do(wxString ans) Action  \
	public: bool Invalidate(void *ptr) { return ptr==Arg; } \
	private: PtrType *Arg; };
#define _DEBUG_LAMBDA_2(Name,PtrType1,Arg1,Type2,Arg2,Action) \
	class Name : public DebugLambda {\
	public: Name(PtrType1 *arg1, Type2 arg2) : Arg1(arg1),Arg2(arg2) {} \
	public: void Do(wxString ans) Action \
	public: bool Invalidate(void *ptr) { return ptr==Arg1; } \
	private: PtrType1 *Arg1; Type2 Arg2; };
#define _DEBUG_LAMBDA_3(Name,PtrType1,Arg1,Type2,Arg2,Type3,Arg3,Action) \
	class Name : public DebugLambda {\
	public: Name(PtrType1 *arg1, Type2 arg2, Type3 arg3) : Arg1(arg1),Arg2(arg2),Arg3(arg3) {} \
	public: void Do(wxString ans) Action \
	public: bool Invalidate(void *ptr) { return ptr==Arg1; } \
	private: PtrType1 *Arg1; Type2 Arg2; Type3 Arg3;};



#define _calc_delay(speed) 25+(100-speed)*20

class DebugManager {
private:
	bool step_in;
	bool subtitles_on;
	bool do_desktop_test;
	mxProcess *process;
	wxString current_proc_name;
	wxString sbuffer;
	mxSource *source;
	wxSocketBase *socket;
	friend class mxProcess;
	friend class mxDebugWindow;
	friend class mxDesktopTestGrid;
	bool should_pause;
	int current_line, current_inst; // linea e instruccion actual, como viene del interprete (base 1), para usar en el subtitulo
public:
	bool debugging, paused;
	DebugManager();
	void Start(mxProcess *process, mxSource *src);
	void Close(mxSource *src);
	void SetSocket(wxSocketBase *s);
	void ProcSocketData(wxString data);
	void ProcessSocketLost();
	void SetSpeed(int speed);
	int GetSpeed(int delay) const;
	bool Pause();
	void Step();
	void Stop();
	void SetStepIn(bool b);
	void SetSubtitles(bool b);

private:
	std::map<wxString,DebugLambda*> on_evaluar;
	void RunLambda(wxString key, wxString ans);
public:
	void SendEvaluation(wxString exp, DebugLambda *lambda);
	void InvalidateLambda(void *ptr); //< para sacar de on_evaluar lambdas que dejen de ser validas
};

extern DebugManager *debug;
#endif

