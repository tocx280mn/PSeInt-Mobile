#ifndef MXTESTPANEL_H
#define MXTESTPANEL_H
#include <wx/panel.h>
#include "../pseval/Package.h"

class wxButton;
class wxStaticText;
class wxSizer;
class mxSource;

class mxTestPanel : public wxPanel {
private:
	static bool first_time; ///< flag to register a new wxMemoryFSHandler only once
	Package pack;
	wxSizer *sizer;
	wxButton *eval_button, *help_button;
	wxStaticText *label;
	wxString path,key;
	mxSource *src;
public:
	mxTestPanel(wxWindow *parent);
	bool Destroy();
	bool Load(const wxString &path, const wxString &key);
	mxSource *GetSrc() { return src; }
	const wxString GetHelp() const;
	void OnShowHideHelp(bool show);
private:
	void Run(const wxString &source_fname);
	void OnRun(wxCommandEvent &event);
	void OnHelp(wxCommandEvent &event);
	DECLARE_EVENT_TABLE();
};

#endif

