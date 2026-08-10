#ifndef MXFONTSCONFIG_H
#define MXFONTSCONFIG_H
#include <wx/dialog.h>

class wxComboBox;
class wxTextCtrl;
class wxStyledTextCtrl;

class mxFontsConfig : public wxDialog {
	bool m_constructed;
	wxComboBox *m_wx_font_name;
	wxComboBox *m_term_font_name;
	wxTextCtrl *m_wx_font_size;
	wxTextCtrl *m_term_font_size;
	wxStyledTextCtrl *m_wx_sample;
	wxStyledTextCtrl *m_term_sample;
	wxTextCtrl *m_print_font_size;
public:
	mxFontsConfig();
	void OnText(wxCommandEvent &evt);
	void OnCombo(wxCommandEvent &evt);
	void OnButtonOk(wxCommandEvent &evt);
	void OnButtonCancel(wxCommandEvent &evt);
	void ApplyToSamples();
	DECLARE_EVENT_TABLE();
};

#endif

