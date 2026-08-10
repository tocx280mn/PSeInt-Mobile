#include "mxAsyncProcess.h"

mxAsyncProcess::mxAsyncProcess() {
}

bool mxAsyncProcess::Run(wxString command, wxArrayString &output){
	if (wxExecute(command, wxEXEC_ASYNC,this)) {
		running=true;
		while (running) {
			wxYield();
		}
		return true;
	} else
		return false;
}


void mxAsyncProcess::OnTerminate(int pid, int status) {
	running=false;
}

mxAsyncProcess::~mxAsyncProcess() {
	
}

