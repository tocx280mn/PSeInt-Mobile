#ifndef MXASYNCPROCESS_H
#define MXASYNCPROCESS_H
#include <wx/process.h>

class mxAsyncProcess : public wxProcess {
private:
	bool running;
public:
	mxAsyncProcess();
	bool Run(wxString command, wxArrayString &output);
	void OnTerminate(int pid, int status);
	~mxAsyncProcess();
};

#endif

