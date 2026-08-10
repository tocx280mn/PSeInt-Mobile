#ifndef MXINPUTDIALOG_H
#define MXINPUTDIALOG_H
#include <wx/dialog.h>

class wxCheckBox;
class wxTextCtrl;
class mxInputDialog : public wxDialog {
private:
	wxCheckBox *m_check_use, *m_check_partial;
	wxTextCtrl *m_values;
	wxString m_old_values;
protected:
public:
	mxInputDialog(wxWindow *parent);
	void Show();
	void OnCheckUse(wxCommandEvent &evt);
	void OnButtonOk(wxCommandEvent &evt);
	void OnButtonCancel(wxCommandEvent &evt);
	void OnButtonLoad(wxCommandEvent &evt);
	void OnButtonSave(wxCommandEvent &evt);
	bool HasInput();
	bool IsPartial();
	wxString GetInput();
	DECLARE_EVENT_TABLE();
};

#endif

