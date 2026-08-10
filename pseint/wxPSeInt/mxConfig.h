#ifndef MXCONFIG_H
#define MXCONFIG_H
#include <wx/dialog.h>
#include <wx/checklst.h>
#include "ConfigManager.h"
#include "../pseint/LangSettings.h"

class mxConfig : public wxDialog {
private:
	wxCheckListBox *m_list;
	void ReadFromStruct(LangSettings l);
	void CopyToStruct(LangSettings &l);
	LangSettings &lang;
public:
	mxConfig(wxWindow *parent, LangSettings &settings);
	void OnClose(wxCloseEvent &evt);
	void OnOpenButton(wxCommandEvent &evt);
	void OnSaveButton(wxCommandEvent &evt);
	void OnOkButton(wxCommandEvent &evt);
	void OnCancelButton(wxCommandEvent &evt);
	static wxString LoadFromFile(wxWindow *parent);
	DECLARE_EVENT_TABLE();
};

#endif

