#ifndef MXOPERSWINDOW_H
#define MXOPERSWINDOW_H
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <vector>

class wxBoxSizer;
class wxButton;
class wxStaticText;

class mxOpersWindow : public wxScrolledWindow {
	struct oper_item {
		wxString code;
		wxButton *but;
		oper_item(wxString _code, wxButton *_but):code(_code),but(_but){}
	};
	struct label_item {
		wxString label;
		wxButton *but;
		bool showing;
		int from,to;
		label_item(wxString _label, wxButton *_but):label(_label),but(_but),showing(true){}
	};
	wxBoxSizer *sizer;
	std::vector<oper_item> lista;
	std::vector<label_item> lista2;
	void Replace(oper_item &o, wxString f1, wxString t1, wxString f2, wxString t2, bool sent);
	void AddCaterory(wxString label);
	void Add(wxString code, wxString label, wxString name, wxString desc);
	void Finish();
public:
	mxOpersWindow(wxWindow *parent);
	void OnItem(wxCommandEvent &evt);
	void OnLabel(wxCommandEvent &evt);
	void AdjustToProfile();
	DECLARE_EVENT_TABLE();
};

extern mxOpersWindow *opers_window;

#endif

