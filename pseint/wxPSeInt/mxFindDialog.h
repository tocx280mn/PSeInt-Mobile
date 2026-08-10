#ifndef MXFINDDIALOG_H
#define MXFINDDIALOG_H
#include <wx/dialog.h>
#include <wx/string.h>
#include <wx/arrstr.h>

class wxComboBox;
class wxCheckBox;
class mxSource;
class wxButton;
class wxStaticText;

class mxFindDialog : public wxDialog {
private:
	DECLARE_EVENT_TABLE();
public:
	bool only_selection;
	int last_flags;
	wxString last_search;
	wxString last_replace;
	wxComboBox *combo_find;
	wxComboBox *combo_replace;
	wxCheckBox *check_case;
	wxCheckBox *check_word;
	wxCheckBox *check_start;
	wxCheckBox *check_close;
	wxButton *replace_button;
	wxButton *next_button;
	wxButton *replace_all_button;
	wxStaticText *replace_static;
	mxFindDialog(wxWindow* parent);
	~mxFindDialog();
	void ShowFind(mxSource *source);
	void ShowReplace(mxSource *source);
	void OnFindNextButton(wxCommandEvent &event);
	void OnFindPrevButton(wxCommandEvent &event);
	void OnReplaceButton(wxCommandEvent &event);
	void OnReplaceAllButton(wxCommandEvent &event);
	bool FindNext();
	bool FindPrev();
};

#endif

