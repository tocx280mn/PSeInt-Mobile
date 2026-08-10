#ifndef MXPROCESS_H
#define MXPROCESS_H
#include <wx/process.h>
#include "mxSource.h"


class wxOutputStream;
class wxInputStream;

class mxProcess : public wxProcess {
private:
	static int cont;
	int command_on_death_time;
	mxSource *source;
//	bool was_readonly;
	wxString temp;
	int what;
	long pid;
	wxString file_code, file_log, file_other, export_lang;
	wxOutputStream *output;
	wxInputStream *input;
	wxString filename;
	friend class mxMainWindow;
	friend class DebugManager;
	friend class mxSource;
public:
	mxProcess *next, *prev;
	mxProcess(mxSource *src);
	bool CheckSyntax(wxString file, wxString extra_args=wxEmptyString);
	bool Run(wxString file, bool check_first=false);
	bool Debug(wxString file, bool check_first=false);
//	bool Draw(wxString file, bool check_first=false);
	bool DrawAndEdit(wxString file, bool check_first=false);
	bool SaveDraw(wxString file, bool check_first=false);
	bool ExportLang(wxString file, wxString lang, bool check_first=false);
	static wxString GetDrawPreArgs();
	static wxString GetDrawPostArgs();
	static wxString GetProfileArgs();
	wxString GetInputArgs();
	~mxProcess();
	void OnTerminate(int pid, int status);
	void SetSourceDeleted();
};

extern mxProcess *proc_list;
extern mxProcess *proc_for_killing;
#endif

