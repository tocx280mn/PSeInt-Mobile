#ifndef MXWELCOME_H
#define MXWELCOME_H
#include <wx/dialog.h>

class mxWelcome : public wxDialog {
public:
	mxWelcome(wxWindow *parent);
	
	void OnProfileFromList(wxCommandEvent &e);
	void OnProfileFromFile(wxCommandEvent &e);
	void OnDefaultProfile(wxCommandEvent &e);
	void OnClose(wxCloseEvent &evt);
//	void OnCustomProfile(wxCommandEvent &e);
private:
	DECLARE_EVENT_TABLE();
};

#endif

