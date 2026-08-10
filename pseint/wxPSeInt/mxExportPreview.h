#ifndef MXEXPORTPREVIEW_H
#define MXEXPORTPREVIEW_H
#include <wx/frame.h>
#include <wx/process.h>

class wxComboBox;
class wxStyledTextCtrl;

class mxExportPreview : public wxFrame {
	wxStyledTextCtrl *code_ctrl;
	wxComboBox *combo_lang;
	wxString temp_filename;
	wxProcess *the_process;
	int pid;
	enum { mxEP_NONE, mxEP_CHECK, mxEP_EXP, mxEP_UPDATE } state;
public:
	mxExportPreview();
	~mxExportPreview();
	void OnButtonCopy(wxCommandEvent &event);
	void OnButtonSave(wxCommandEvent &event);
	void OnButtonUpdate(wxCommandEvent &event);
	void OnComboLang(wxCommandEvent &event);
	void OnClose(wxCloseEvent &event);
	void UpdatePrev();
	void OnProcTerminate(wxProcessEvent &event);
	void SetMessage(const wxString &msg);
	DECLARE_EVENT_TABLE();
};

extern mxExportPreview *export_preview;

#endif

